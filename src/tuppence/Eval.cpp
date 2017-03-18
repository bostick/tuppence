//===------ Eval.cpp ------------------------------------------------------===//
//
// The Tuppence Programming Language
//
// This file is distributed under the MIT Open Source License.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

#include "Eval.h"

#include "Logger.h"
#include "Parser.h"

#include "llvm/Support/ErrorHandling.h"

#include <iostream>

namespace tuppence {

	namespace eval {

		const FiniteWord EMPTY = FiniteWord::FactoryEmpty();
		const FiniteWord ZERO_1BIT = FiniteWord::FactoryBool((bool)0);
		const FiniteWord ONE_1BIT = FiniteWord::FactoryBool((bool)1);

		const Symbol SYMBOL_INFINITY = Symbol("infinity");
		const BuiltinFunction BUILTIN_PRINT = BuiltinFunction(&tuppence::eval::print);
		const BuiltinFunction BUILTIN_EXIT = BuiltinFunction(&tuppence::eval::exit);
		const BuiltinFunction BUILTIN_RATIONALIZE = BuiltinFunction(&tuppence::eval::rationalize);

	}

}

using namespace tuppence;

std::map<std::string, std::shared_ptr<Value>> eval::NamedValues;

const std::shared_ptr<Value> ExpressionListExprAST::eval() const {
	std::vector<std::shared_ptr<Value>> Vals;
	for (auto& Expr : Exprs) {
		auto Val = Expr->eval();
		if (!Val) {
			return nullptr;
		}
		// flatten out all child value lists
		if (auto ValList = llvm::dyn_cast<ValueList>(Val.get())) {
			for (auto& V : *ValList) {
				Vals.push_back(V);
			}
		}
		else {
			Vals.push_back(Val);
		}
	}
	return std::make_shared<ValueList>(Vals);
}

/// Last part of sequence is returned
const std::shared_ptr<Value> ExpressionSequenceExprAST::eval() const {
	if (Exprs.empty()) {
		return std::make_shared<FiniteWord>(eval::EMPTY);
	}
	auto Iter = Exprs.begin();
	for (; Iter != --Exprs.end(); Iter++) {
		auto Expr = *Iter;
        auto Val = Expr->eval();
		if (!Val) {
			return nullptr;
		}
	}

	auto Last = *Iter;
	auto LastVal = Last->eval();
	if (!LastVal) {
		return nullptr;
	}
	return LastVal;
}

const std::shared_ptr<Value> RationalWordExprAST::eval() const {
    return std::make_shared<RationalWord>(Val);
}

const std::shared_ptr<Value> FiniteWordExprAST::eval() const {
	return std::make_shared<FiniteWord>(Val);
}

const std::shared_ptr<Value> IdentifierExprAST::eval() const {
	auto V = eval::NamedValues[Name];
	if (!V) {
		return LogError("Unknown variable name: " + Name);
	}
	return V;
}

const std::shared_ptr<Value> UnaryExprAST::eval() const {
	auto OperandValue = Operand->eval();
	if (!OperandValue) {
		return nullptr;
	}

	// For arithmetic, bitwise, and structural operations, concatenate all lists together
	if (auto ListOperand = llvm::dyn_cast<ValueList>(OperandValue.get())) {
		OperandValue = ListOperand->concatenate();
		if (!OperandValue) {
			return nullptr;
		}
	}

	switch (Opcode) {
	case '-': {
		if (auto FiniteWordOperand = llvm::dyn_cast<FiniteWord>(OperandValue.get())) {
			return std::make_shared<FiniteWord>(-(*FiniteWordOperand));
		}
		else if (auto RationalWordOperand = llvm::dyn_cast<RationalWord>(OperandValue.get())) {
			return std::make_shared<RationalWord>(-(*RationalWordOperand));
		}
		return LogError("Invalid type for operator -: " + stringFromValueKind(OperandValue->getKind()));
	}
	case tok_ddd: {
		if (auto FiniteWordOperand = llvm::dyn_cast<FiniteWord>(OperandValue.get())) {
			if (FiniteWordOperand->getSize() == 0) {
				return std::make_shared<FiniteWord>();
			}
			else {
				return std::make_shared<RationalWord>(RationalWord::FactoryPeriodTransient(*FiniteWordOperand, eval::EMPTY));
			}
		}
		return LogError("Invalid type for operator ...: " + stringFromValueKind(OperandValue->getKind()));
	}
	case '~': {
		if (auto FiniteWordOperand = llvm::dyn_cast<FiniteWord>(OperandValue.get())) {
			return std::make_shared<FiniteWord>(FiniteWord::bitwise_not(*FiniteWordOperand));
		}
		else if (auto RationalWordOperand = llvm::dyn_cast<RationalWord>(OperandValue.get())) {
			return std::make_shared<RationalWord>(RationalWord::bitwise_not(*RationalWordOperand));
		}
		return LogError("Invalid type for operator ~: " + stringFromValueKind(OperandValue->getKind()));
	}
	default:
		return LogError("Unary operator not found: " + stringFromToken(Opcode));
	}
}

const std::shared_ptr<Value> BinaryExprAST::eval() const {
	// Special case '=' because we don't want to emit the LHS as an expression.
	if (Op == '=') {
		if (auto IdentifierLHS = llvm::dyn_cast<IdentifierExprAST>(LHS.get())) {
			// eval the RHS.

			auto Val = RHS->eval();
			if (!Val) {
				return nullptr;
			}

	 		if (auto ListVal = llvm::dyn_cast<ValueList>(Val.get())) {
			
				eval::NamedValues[IdentifierLHS->getName()] = (*ListVal)[0];
			
				return std::make_shared<FiniteWord>();
			}
			else {
				// regular, non-ValueList
				eval::NamedValues[IdentifierLHS->getName()] = Val;

				return std::make_shared<FiniteWord>();
			}
		}
		else if (auto ListLHS = llvm::dyn_cast<ExpressionListExprAST>(LHS.get())) {

			auto Val = RHS->eval();
			if (!Val) {
				return nullptr;
			}

			if (auto ListVal = llvm::dyn_cast<ValueList>(Val.get())) {

				auto Exprs = ListLHS->getExprs();

				if (Exprs.size() <= ListVal->size()) {

					for (size_t i = 0; i < Exprs.size(); i++) {
						auto Var = Exprs[i];
						if (auto V = llvm::dyn_cast<IdentifierExprAST>(Var.get())) {
							eval::NamedValues[V->getName()] = (*ListVal)[i];
						}
						else {
							return LogError("Expected identifer: " + Var->string());
						}
					}

					return std::make_shared<FiniteWord>();
				}
				else {
					return LogError("Too many identifiers for assignment");
				}
			}
			else {
				return LogError("List expected on RHS: " + RHS->string());
			}
		}
		else {
			return LogError("Invalid LHS for =: " + LHS->string());
		}
	}

	auto L = LHS->eval();
	if (!L) {
		return nullptr;
	}
	// if L is a list, then concatenate all together
	if (auto ListL = llvm::dyn_cast<ValueList>(L.get())) {
		L = ListL->concatenate();
		if (!L) {
			return nullptr;
		}
	}

	// handle method calls first
	if (Op == '.') {
		if (auto IdentifierRHS = llvm::dyn_cast<IdentifierExprAST>(RHS.get())) {
			auto Name = IdentifierRHS->getName();
			if (auto FiniteWordL = llvm::dyn_cast<FiniteWord>(L.get())) {
				if (Name == "size") {
					return std::make_shared<RationalWord>(RationalWord::FactorySizeT(FiniteWordL->getSize()));
				}
				else {
					return LogError("Unrecognized method for FiniteWord: " + Name);
				}
			}
			else if (auto RationalWordL = llvm::dyn_cast<RationalWord>(L.get())) {
				if (Name == "period") {
					return std::make_shared<FiniteWord>(RationalWordL->getPeriod());
				} else if (Name == "transient") {
					return std::make_shared<FiniteWord>(RationalWordL->getTransient());
				}
				else if (Name == "size") {
					auto Infinity = eval::NamedValues["infinity"];
					return Infinity;
				}
				else if (Name == "numerator") {
					auto Numerator = RationalWordL->getNumerator();
					return std::make_shared<RationalWord>(Numerator);
				}
				else if (Name == "denominator") {
					auto Denominator = RationalWordL->getDenominator();
					return std::make_shared<RationalWord>(Denominator);
				}
				else {
					return LogError("Unrecognized method for RationalWord: " + Name);
				}
			} else {
				return LogError("Invalid LHS for .: " + L->string());
			}
		}
		else {
			return LogError("Invalid RHS for .: " + RHS->string());
		}
	}

	auto R = RHS->eval();
	if (!R) {
		return nullptr;
	}

	// For arithmetic and structural operations, concatenate all lists together
	if (auto ListR = llvm::dyn_cast<ValueList>(R.get())) {
		R = ListR->concatenate();
		if (!R) {
			return nullptr;
		}
	}

	switch (Op) {
	case tok_pp: {
		if (auto RationalWordL = llvm::dyn_cast<RationalWord>(L.get())) {
			if (auto RationalWordR = llvm::dyn_cast<RationalWord>(R.get())) {
				if (!RationalWordR->isNonNegativeInteger()) {
					return LogError("Expected non-negative integer on RHS of %%: " + R->string());
				}
				auto i = RationalWordR->getUInt64Value();
				return std::make_shared<FiniteWord>(RationalWordL->residue(i));
			} else if (auto SymbolR = llvm::dyn_cast<Symbol>(R.get())) {
				if (*SymbolR == eval::SYMBOL_INFINITY) {
					return L;
				}
			}
		}
		else if (auto FiniteWordL = llvm::dyn_cast<FiniteWord>(L.get())) {
			if (auto RationalWordR = llvm::dyn_cast<RationalWord>(R.get())) {
				if (!RationalWordR->isNonNegativeInteger()) {
					return LogError("Expected non-negative integer on RHS of %%: " + R->string());
				}
				auto i = RationalWordR->getUInt64Value();
				if (i > FiniteWordL->getSize()) {
					return LogError("Requested residue is too large: " + L->string() + " " + R->string());
				}
				return std::make_shared<FiniteWord>(FiniteWordL->residue(i));
			}
			else if (auto SymbolR = llvm::dyn_cast<Symbol>(R.get())) {
				if (*SymbolR == eval::SYMBOL_INFINITY) {
					return L;
				}
			}
		}
		return LogError("Incorrect types for %%: " + L->string() + " " + R->string());
	}
	case tok_gtgt: {
		if (auto RationalWordL = llvm::dyn_cast<RationalWord>(L.get())) {
			if (auto RationalWordR = llvm::dyn_cast<RationalWord>(R.get())) {
				if (!RationalWordR->isNonNegativeInteger()) {
					return LogError("Expected non-negative integer on RHS of >>: " + R->string());
				}
				auto i = RationalWordR->getUInt64Value();
				return std::make_shared<RationalWord>(RationalWordL->shiftRight(i));
			}
		}
		else if (auto FiniteWordL = llvm::dyn_cast<FiniteWord>(L.get())) {
			if (auto RationalWordR = llvm::dyn_cast<RationalWord>(R.get())) {
				if (!RationalWordR->isNonNegativeInteger()) {
					return LogError("Expected non-negative integer on RHS of >>: " + R->string());
				}
				auto i = RationalWordR->getUInt64Value();
				if (i > FiniteWordL->getSize()) {
					return LogError("Requested shift is too large: " + L->string() + " " + R->string());
				}
				return std::make_shared<FiniteWord>(FiniteWordL->shiftRight(i));
			}
		}
		return LogError("Incorrect types for >>: " + L->string() + " " + R->string());
	}
	case tok_gtp: {
		if (auto RationalWordL = llvm::dyn_cast<RationalWord>(L.get())) {
			if (auto RationalWordR = llvm::dyn_cast<RationalWord>(R.get())) {
				if (!RationalWordR->isNonNegativeInteger()) {
					return LogError("Expected non-negative integer on RHS of >%: " + R->string());
				}
				auto i = RationalWordR->getUInt64Value();
				RationalWord Hi;
				FiniteWord Lo;
				RationalWordL->shiftRightResidue(i, Hi, Lo);
				std::vector<std::shared_ptr<Value>> Vec = { std::make_shared<FiniteWord>(Lo), std::make_shared<RationalWord>(Hi) };
				return std::make_shared<ValueList>(Vec);
			}
		}
		else if (auto FiniteWordL = llvm::dyn_cast<FiniteWord>(L.get())) {
			if (auto RationalWordR = llvm::dyn_cast<RationalWord>(R.get())) {
				if (!RationalWordR->isNonNegativeInteger()) {
					return LogError("Expected non-negative integer on RHS of >%: " + R->string());
				}
				auto i = RationalWordR->getUInt64Value();
				if (i > FiniteWordL->getSize()) {
					return LogError("Requested shift is too large: " + L->string() + " " + R->string());
				}
				FiniteWord Hi;
				FiniteWord Lo;
				FiniteWordL->shiftRightResidue(i, Hi, Lo);
				std::vector<std::shared_ptr<Value>> Vec = { std::make_shared<FiniteWord>(Lo), std::make_shared<FiniteWord>(Hi) };
				return std::make_shared<ValueList>(Vec);
			}
		}
		return LogError("Incorrect types for >%: " + L->string() + " " + R->string());
	}
	case '-': {
		if (auto FiniteWordL = llvm::dyn_cast<FiniteWord>(L.get())) {
			if (auto FiniteWordR = llvm::dyn_cast<FiniteWord>(R.get())) {
				if (FiniteWordL->getSize() != FiniteWordR->getSize()) {
					return LogError("Expected same sizes for -: " + L->string() + " " + R->string());
				}
				if (FiniteWordL->getSize() == 0) {
					return LogError("Expected positive sizes for -: " + L->string() + " " + R->string());
				}
				FiniteWord Lo;
				FiniteWord Hi;
				FiniteWord::minus(*FiniteWordL, *FiniteWordR, Hi, Lo);
				std::vector<std::shared_ptr<Value>> Vals = { std::make_shared<FiniteWord>(Lo), std::make_shared<FiniteWord>(Hi) };
				return std::make_shared<ValueList>(Vals);
			}
			else {
				return LogError("Invalid types for -: " + L->string() + " " + R->string());
			}
		}
		else if (auto RationalWordL = llvm::dyn_cast<RationalWord>(L.get())) {
			if (auto RationalWordR = llvm::dyn_cast<RationalWord>(R.get())) {
				return std::make_shared<RationalWord>(RationalWord::minus(*RationalWordL, *RationalWordR));
			}
			else {
				return LogError("Invalid types for -: " + L->string() + " " + R->string());
			}
		}
		else {
			return LogError("Invalid types for -: " + L->string() + " " + R->string());
		}
	}
	case '/': {
		if (auto FiniteWordL = llvm::dyn_cast<FiniteWord>(L.get())) {
			if (auto FiniteWordR = llvm::dyn_cast<FiniteWord>(R.get())) {
				if (FiniteWordL->getSize() != FiniteWordR->getSize()) {
					return LogError("Expected same sizes for /: " + L->string() + " " + R->string());
				}
				if (FiniteWordL->getSize() == 0) {
					return LogError("Expected positive sizes for /: " + L->string() + " " + R->string());
				}
				FiniteWord Remainder;
				FiniteWord Quotient;
				FiniteWord::divide(*FiniteWordL, *FiniteWordR, Remainder, Quotient);
				std::vector<std::shared_ptr<Value>> Vals = { std::make_shared<FiniteWord>(Quotient), std::make_shared<FiniteWord>(Remainder) };
				return std::make_shared<ValueList>(Vals);
			}
			else {
				return LogError("Invalid types for /: " + L->string() + " " + R->string());
			}
		}
		else if (auto RationalWordL = llvm::dyn_cast<RationalWord>(L.get())) {
			if (auto RationalWordR = llvm::dyn_cast<RationalWord>(R.get())) {
				if (RationalWordR->residue(1) == eval::ZERO_1BIT) {
					return LogError("Divisor cannot have 0 first bit for /: " + R->string());
				}
				return std::make_shared<RationalWord>(RationalWord::divide(*RationalWordL, *RationalWordR));
			}
			else {
				return LogError("Invalid types for /: " + L->string() + " " + R->string());
			}
		}
		else {
			return LogError("Invalid types for /: " + L->string() + " " + R->string());
		}
	}
	default:
		return LogError("Binary operator not found: " + stringFromToken(Op));
	}
}

const std::shared_ptr<Value> InfixExprAST::eval() const {

	std::vector<std::shared_ptr<Value>> Vals;
	for (auto& Expr : Args) {
		auto Val = Expr->eval();
		if (!Val) {
			return nullptr;
		}
		Vals.push_back(Val);
	}

	if (Op == tok_eqeq || Op == tok_beq) {
		// == and != operators are special, if any word values are present, then only consider
		// the first element of any lists
		// if all arguments are lists, then compare element-wise
		// functions are not comparable to anything

		// first determine if any are FiniteWords, and the necessary size
		auto finiteWordIsPresent(false);
		auto rationalWordIsPresent(false);
		size_t finiteWordSize(0);
		size_t valueListSize(0);
		for (auto& Val : Vals) {
			if (auto FiniteWordVal = llvm::dyn_cast<FiniteWord>(Val.get())) {
				finiteWordIsPresent = true;
				// use first size that is encountered
				finiteWordSize = FiniteWordVal->getSize();
			}
			else if (/*auto RationalWordVal = */llvm::dyn_cast<RationalWord>(Val.get())) {
				rationalWordIsPresent = true;
			}
			else if (auto ValueListVal = llvm::dyn_cast<ValueList>(Val.get())) {
				valueListSize = ValueListVal->size();
			}
		}

		// if FiniteWords are present, then check sizes are correct
		// ensure only FiniteWords and RationalWords are present
		if (finiteWordIsPresent) {
			std::vector<FiniteWord> FiniteWordVals;

			for (auto& Val : Vals) {
				if (auto FiniteWordVal = llvm::dyn_cast<FiniteWord>(Val.get())) {
					if (FiniteWordVal->getSize() != finiteWordSize) {
						return LogError("Incorrect size for +:" + Val->string());
					}
					FiniteWordVals.push_back(*FiniteWordVal);
				}
				else if (auto ValueListVal = llvm::dyn_cast<ValueList>(Val.get())) {
					auto First = (*ValueListVal)[0];
					if (auto FiniteWordVal = llvm::dyn_cast<FiniteWord>(First.get())) {
						if (FiniteWordVal->getSize() != finiteWordSize) {
							return LogError("Incorrect size for +:" + First->string());
						}
						FiniteWordVals.push_back(*FiniteWordVal);
					}
				}
				else {
					return LogError("Invalid type for +:" + Val->string());
				}
			}

			switch (Op) {
			case tok_eqeq: return std::make_shared<FiniteWord>(FiniteWord::equal(FiniteWordVals));
			case tok_beq: return std::make_shared<FiniteWord>(FiniteWord::notequal(FiniteWordVals));
			default:
				llvm_unreachable("fallthrough");
			}
		}
		else if (rationalWordIsPresent) {
			std::vector<RationalWord> RationalWordVals;

			// ensure only RationalWords are present
			for (auto& Val : Vals) {
				if (auto RationalWordVal = llvm::dyn_cast<RationalWord>(Val.get())) {
					RationalWordVals.push_back(*RationalWordVal);
				}
				else if (auto ValueListVal = llvm::dyn_cast<ValueList>(Val.get())) {
					auto First = ValueListVal[0];
					if (auto RationalWordVal = llvm::dyn_cast<RationalWord>(&First)) {
						RationalWordVals.push_back(*RationalWordVal);
					}
				}
				else {
					return LogError("Invalid type for +:" + Val->string());
				}
			}

			switch (Op) {
			case tok_eqeq: return std::make_shared<FiniteWord>(RationalWord::equal(RationalWordVals));
			case tok_beq: return std::make_shared<FiniteWord>(RationalWord::notequal(RationalWordVals));
			default:
				llvm_unreachable("fallthrough");
			}
		}
		else {
			// assume comparing lists element-wise
			std::vector<ValueList> ValueListVals;

			// ensure only ValueLists are present
			for (auto& Val : Vals) {
				if (auto ValueListVal = llvm::dyn_cast<ValueList>(Val.get())) {
					if (ValueListVal->size() != valueListSize) {
						return LogError("Incorrect list size for +:" + Val->string());
					}
					if (!ValueListVal->isComparable()) {
						return LogError("List contains non-comparable object: " + Val->string());
					}
					ValueListVals.push_back(*ValueListVal);
				}
				else {
					return LogError("Invalid type for +:" + Val->string());
				}
			}

			switch (Op) {
			case tok_eqeq: return std::make_shared<FiniteWord>(ValueList::equal(ValueListVals));
			case tok_beq: return std::make_shared<FiniteWord>(ValueList::notequal(ValueListVals));
			default:
				llvm_unreachable("fallthrough");
			}
		}
	}

	// For arithmetic and bitwise operations, concatenate all lists together
	{
		std::vector<std::shared_ptr<Value>> FlattenedVals;
		for (auto i = 0; i < Vals.size(); i++) {
			auto Val = Vals[i];
			if (auto ListVal = llvm::dyn_cast<ValueList>(Val.get())) {
				auto FlatVal = ListVal->concatenate();
				if (!FlatVal) {
					return nullptr;
				}
				FlattenedVals.push_back(FlatVal);
			}
			else {
				FlattenedVals.push_back(Val);
			}
		}
		Vals = FlattenedVals;
	}

	if (Op == '#') {
		return ValueList(Vals).concatenate();
	}

	// Now, only remaining operators are those that cast to either FiniteWord or RationalWord

	if (Op == '+' ||
		Op == '*' ||
		Op == '|' ||
		Op == '&' ||
		Op == '^') {
		// first determine if any are FiniteWords, and the necessary size
		auto finiteWordIsPresent(false);
		auto rationalWordIsPresent(false);
		size_t finiteWordSize(0);
		for (auto& Val : Vals) {
			if (auto FiniteWordVal = llvm::dyn_cast<FiniteWord>(Val.get())) {
				finiteWordIsPresent = true;
				// use first size that is encountered
				finiteWordSize = FiniteWordVal->getSize();
			}
			else if (/*auto RationalWordVal = */llvm::dyn_cast<RationalWord>(Val.get())) {
				rationalWordIsPresent = true;
			}
		}

		// if FiniteWords are present, then check sizes are correct
		// ensure only FiniteWords and RationalWords are present
		if (finiteWordIsPresent) {
			std::vector<FiniteWord> FiniteWordVals;

			for (auto& Val : Vals) {
				if (auto FiniteWordVal = llvm::dyn_cast<FiniteWord>(Val.get())) {
					if (FiniteWordVal->getSize() != finiteWordSize) {
						return LogError("Invalid size: " + Val->string());
					}
					FiniteWordVals.push_back(*FiniteWordVal);
				}
				else {
					return LogError("Invalid type: " + Val->string());
				}
			}

			switch (Op) {
			case '+': {
				FiniteWord Lo;
				FiniteWord Hi;
				FiniteWord::plus(FiniteWordVals, Hi, Lo);
				std::vector<std::shared_ptr<Value>> Vals = { std::make_shared<FiniteWord>(Lo), std::make_shared<FiniteWord>(Hi) };
				return std::make_shared<ValueList>(Vals);
			}
			case '*': {
				FiniteWord Lo;
				FiniteWord Hi;
				FiniteWord::times(FiniteWordVals, Hi, Lo);
				std::vector<std::shared_ptr<Value>> Vals = { std::make_shared<FiniteWord>(Lo), std::make_shared<FiniteWord>(Hi) };
				return std::make_shared<ValueList>(Vals);
			}
			case '|': return std::make_shared<FiniteWord>(FiniteWord::bitwise_or(FiniteWordVals));
			case '&': return std::make_shared<FiniteWord>(FiniteWord::bitwise_and(FiniteWordVals));
			case '^': return std::make_shared<FiniteWord>(FiniteWord::bitwise_xor(FiniteWordVals));
			default:
				llvm_unreachable("fallthrough");
			}
		}
		else if (rationalWordIsPresent) {
			std::vector<RationalWord> RationalWordVals;

			// ensure only RationalWords are present
			for (auto& Val : Vals) {
				if (auto RationalWordVal = llvm::dyn_cast<RationalWord>(Val.get())) {
					RationalWordVals.push_back(*RationalWordVal);
				}
				else {
					return LogError("Invalid type: " + Val->string());
				}
			}

			switch (Op) {
			case '+': return std::make_shared<RationalWord>(RationalWord::plus(RationalWordVals));
			case '*': return std::make_shared<RationalWord>(RationalWord::times(RationalWordVals));
			case '|': return std::make_shared<RationalWord>(RationalWord::bitwise_or(RationalWordVals));
			case '&': return std::make_shared<RationalWord>(RationalWord::bitwise_and(RationalWordVals));
			case '^': return std::make_shared<RationalWord>(RationalWord::bitwise_xor(RationalWordVals));
			default:
				llvm_unreachable("fallthrough");
			}
		}
		else {
			return LogError("Cannot operate on values");
		}
	}

	return LogError("Infix operator not found: " + stringFromToken(Op));
}

const std::shared_ptr<Value> CallExprAST::eval() const {

	std::vector<std::shared_ptr<Value>> ArgsV;
	auto Args = getArgs()->getExprs();
	for (auto& Arg : Args) {
		auto V = Arg->eval();
		if (!V) {
			return nullptr;
		}

		ArgsV.push_back(V);
	}

	// Look up the name in the global module table.
	auto Named = eval::NamedValues[Callee->getName()];
	if (!Named) {
		return LogError("Unknown function referenced: " + Callee->getName());
	}

	if (auto BultinCallee = llvm::dyn_cast<BuiltinFunction>(Named.get());) {
		return BultinCallee->call(ArgsV);
	}

	if (auto UserCalee = llvm::dyn_cast<UserFunction>(Named.get());) {
		return UserCalee->call(ArgsV);
	}

	return LogError("Neither builtin function nor user function: " + Callee->getName());
}

const std::shared_ptr<Value> IfExprAST::eval() const {
	auto CondV = Cond->eval();
	if (!CondV) {
		return nullptr;
	}

	// if Cond is a list, then concatenate all values
	if (auto ListCond = llvm::dyn_cast<ValueList>(CondV.get())) {
		CondV = ListCond->concatenate();
		if (!CondV) {
			return nullptr;
		}
	}

	auto Test = llvm::dyn_cast<FiniteWord>(CondV.get());
	if (!Test) {
		return LogError("Test is not a FiniteWord: " + Test->string());
	}

	if (Test->getSize() != 1) {
		return LogError("Test is not size 1: " + Test->string());
	}

	if (*Test == eval::ONE_1BIT) {
		return Then->eval();
	}
	else {
		return Else->eval();
	}
}

const std::shared_ptr<Value> ForExprAST::eval() const {

	auto StartV = Start->eval();
	if (!StartV) {
		return nullptr;
	}

	std::shared_ptr<Value> Res = std::make_shared<FiniteWord>();

	while (1) {
		auto EV = End->eval();
		if (!EV) {
			return nullptr;
		}

		auto EndV = llvm::dyn_cast<FiniteWord>(EV.get());
		if (!EndV) {
			return LogError("End is not a FiniteWord: " + EndV->string());
		}

		if (EndV->getSize() != 1) {
			return LogError("End is not size 1: " + EndV->string());
		}

		if (*EndV == eval::ZERO_1BIT) {
			break;
		}

		if (!(Res = Body->eval())) {
			return nullptr;
		}

		if (!(Step->eval())) {
			return nullptr;
		}
	}

	return std::make_shared<FiniteWord>();
}

const std::shared_ptr<Value> WhileExprAST::eval() const {
	while (1) {

		auto TestV = Test->eval();
		if (!TestV) {
			return nullptr;
		}

		if (auto FiniteWordTest = llvm::dyn_cast<FiniteWord>(TestV.get())) {
			if (FiniteWordTest->getSize() == 1) {
				if (*FiniteWordTest == eval::ZERO_1BIT) {
					break;
				}
			}
			else {
				return LogError("Test is not size 1: " + FiniteWordTest->string());
			}
		}
		else {
			return LogError("Test is not a FiniteWord: " + TestV->string());
		}

		auto Res = Body->eval();
		if (!Res) {
			return nullptr;
		}
	}

	return std::make_shared<FiniteWord>();
}

const std::shared_ptr<Value> VarExprAST::eval() const {
	std::vector<std::shared_ptr<Value>> Values;

	for (auto& Val : Vals) {
		auto ValEvaled = Val->eval();
		if (!ValEvaled) {
			return nullptr;
		}

		// flatten out ValueList
		if (auto ValList = llvm::dyn_cast<ValueList>(ValEvaled.get())) {
			for (auto& V : *ValList) {
				Values.push_back(V);
			}
		}
		else {
			Values.push_back(ValEvaled);
		}
	}

	if (Vars.size() > Values.size()) {
		return LogError("Wrong # of identifiers and values");
	}

	// Push bindings
	std::vector<std::shared_ptr<Value>> OldBindings;
	for (size_t i = 0, e = Vars.size(); i != e; ++i) {
		auto VarName = Vars[i];
		auto Init = Values[i];
		OldBindings.push_back(eval::NamedValues[VarName->getName()]);
		eval::NamedValues[VarName->getName()] = Init;
	}

	auto BodyVal = Body->eval();
	if (!BodyVal) {
		return nullptr;
	}

	// Pop bindings
	for (size_t i = 0, e = Vars.size(); i != e; ++i) {
		auto VarName = Vars[i];
		eval::NamedValues[VarName->getName()] = OldBindings[i];
	}

	return BodyVal;
}

const std::shared_ptr<Value> DefinitionAST::eval() const {

	auto F = std::make_shared<UserFunction>(Proto, Body);
	
	eval::NamedValues[Proto->getName()->getName()] = F;

	return std::make_shared<FiniteWord>();
}

const std::shared_ptr<Value> UserFunction::call(std::vector<std::shared_ptr<Value>> Args) const {
	if (Proto->getParams().size() != Args.size()) {
		return LogError("Incorrect # arguments passed");
	}

	// Push all bindings
	std::vector<std::shared_ptr<Value>> OldBindings;
	for (size_t i = 0, e = Args.size(); i != e; ++i) {
		auto VarName = Proto->getParams()[i];
		auto Val = Args[i];
		OldBindings.push_back(eval::NamedValues[VarName->getName()]);
		eval::NamedValues[VarName->getName()] = Val;
	}

	auto Res = Body->eval();
	if (!Res) {
		return nullptr;
	}

	// Pop bindings back
	for (size_t i = 0, e = Args.size(); i != e; ++i) {
		auto VarName = Proto->getParams()[i];
		eval::NamedValues[VarName->getName()] = OldBindings[i];
	}

	return Res;
}

const std::shared_ptr<Value> BuiltinFunction::call(std::vector<std::shared_ptr<Value>> Args) const {

	auto Res = (*FunctionPointer)(Args);
	if (!Res) {
		return nullptr;
	}

	return Res;
}

//
// functions
//

// printing is reversed from normal, i.e., printing is done right-to-left
const std::shared_ptr<Value> eval::print(std::vector<std::shared_ptr<Value>> Args) {
	for (auto Iter = Args.rbegin(); Iter != Args.rend(); Iter++) {
		std::cout << (*Iter)->string() << "\n";
	}

    return std::make_shared<FiniteWord>();
}

const std::shared_ptr<Value> eval::exit(std::vector<std::shared_ptr<Value>> Args) {
	if (Args.size() != 0) {
		return LogError("Incorrect # arguments passed");
	}

	std::exit(EXIT_SUCCESS);
	return nullptr;
}

const std::shared_ptr<Value> eval::rationalize(std::vector<std::shared_ptr<Value>> Args) {
	if (Args.size() != 1) {
		return LogError("Incorrect # arguments passed");
	}

	auto Val = Args[0];

	if (auto FiniteWordVal = llvm::dyn_cast<FiniteWord>(Val.get())) {
		if (FiniteWordVal->getSize() == 0) {
			return LogError("Cannot rationalize empty word");
		}

		FiniteWord Hi;
		FiniteWord Lo;
		FiniteWordVal->shiftRightResidue(FiniteWordVal->getSize() - 1, Hi, Lo);
		auto Best = RationalWord::FactoryPeriodTransient(Hi, Lo);

		for (auto i = 0; i < FiniteWordVal->getSize() - 1; i++) {
			FiniteWordVal->shiftRightResidue(i, Hi, Lo);
			auto Res = RationalWord::FactoryPeriodTransient(Hi, Lo);
			if (Res.getPeriod().getSize() + Res.getTransient().getSize() < Best.getPeriod().getSize() + Best.getTransient().getSize()) {
				Best = Res;
			}
		}

		return std::make_shared<RationalWord>(Best);

	}
	else {
		return LogError("Expected FiniteWord: " + Val->string());
	}
}
