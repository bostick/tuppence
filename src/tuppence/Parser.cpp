//===------ Parser.cpp ----------------------------------------------------===//
//
// The Tuppence Programming Language
//
// This file is distributed under the MIT Open Source License.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

#include "Parser.h"

#include "Logger.h"

#include "llvm/Support/ErrorHandling.h"

#include <cassert>
#include <iostream>

using namespace tuppence;

Parser::Parser(std::istream &In) :
	Lex(Lexer(In)) {
	// 1 is lowest precedence.
	BinopPrecedence['='] = 3;
	BinopPrecedence[tok_pp] = 4;

	BinopPrecedence['|'] = 7;
	BinopPrecedence['^'] = 8;
	BinopPrecedence['&'] = 9;
	BinopPrecedence[tok_eqeq] = 10;
	BinopPrecedence[tok_beq] = 10;
	BinopPrecedence[tok_gtgt] = 19;
	BinopPrecedence[tok_gtp] = 19;
	BinopPrecedence['+'] = 20;
	BinopPrecedence['-'] = 20;
	BinopPrecedence['*'] = 40;
	BinopPrecedence['/'] = 40;
	BinopPrecedence['#'] = 50;
	BinopPrecedence['.'] = 60;
	// highest.
}

int Parser::GetTokPrecedence() {

	// return -1 if an error
	// \n is not an error, so return some negative and not -1.
	if (Lex.CurTok == '\n' ||
		Lex.CurTok == ')' ||
		Lex.CurTok == '}' ||
		Lex.CurTok == tok_else ||
		Lex.CurTok == ','
		) {
		return -2;
	}

	// Make sure it's a declared binop.
	int TokPrec = BinopPrecedence[Lex.CurTok];
	if (TokPrec == 0) {
		return -1;
	}
	return TokPrec;
}

const std::shared_ptr<RationalWordExprAST> Parser::ParseRationalWordExpr() {
	assert(Lex.CurTok == tok_rationalword && "Expected tok_rationalword");
	auto Result = std::make_shared<RationalWordExprAST>(*Lex.RationalWordVal);
	Lex.readNextToken(); // consume the number
	return Result;
}

const std::shared_ptr<FiniteWordExprAST> Parser::ParseFiniteWordExpr() {
	assert(Lex.CurTok == tok_finiteword && "Expected tok_finiteword");
	auto Result = std::make_shared<FiniteWordExprAST>(*Lex.FiniteWordVal);
	Lex.readNextToken(); // consume the `
	return Result;
}

const std::shared_ptr<ExpressionListExprAST> Parser::ParseParenExpr() {
	assert(Lex.CurTok == '(' && "Expected (");
	Lex.readNextToken(); // eat (.

	std::vector<std::shared_ptr<ExprAST>> Exprs;
	if (Lex.CurTok != ')') {
		while (1) {
			
			while (Lex.CurTok == '\n') {
				Lex.readNextToken();
			}

			auto E = ParseExpression();
			if (!E) {
				return nullptr;
			}
			// parsing is reversed from normal
			Exprs.insert(Exprs.begin(), E);

			while (Lex.CurTok == '\n') {
				Lex.readNextToken();
			}

			if (Lex.CurTok == ',') {
				Lex.readNextToken(); // eat ','
				continue;
			}
			else if (Lex.CurTok == ')') {
				break;
			}
			else {
				return LogError("unexpected token in Paren expr: " + stringFromToken(Lex.CurTok));
			}
		}
	}
	if (Lex.CurTok != ')') {
		return LogError("expected ')'");
	}
	Lex.readNextToken(); // eat ).

	return std::make_shared<ExpressionListExprAST>(Exprs);
}

// expressions separated by \n
const std::shared_ptr<ExpressionSequenceExprAST> Parser::ParseCurlyExpr() {
	assert(Lex.CurTok == '{' && "Expected {");
	Lex.readNextToken(); // eat {.

	std::vector<std::shared_ptr<ExprAST>> Exprs;
	if (Lex.CurTok != '}') {
		while (1) {

			while (Lex.CurTok == '\n') {
				Lex.readNextToken();
			}

			auto E = ParseExpression();
			if (!E) {
				return nullptr;
			}
			Exprs.push_back(E);

			while (Lex.CurTok == '\n') {
				Lex.readNextToken();
			}

			if (Lex.CurTok == '}') {
				break;
			}
		}
	}
	if (Lex.CurTok != '}') {
		return LogError("expected '}'");
	}
	Lex.readNextToken(); // eat ).

	return std::make_shared<ExpressionSequenceExprAST>(Exprs);
}

// variable |
// function(args)
const std::shared_ptr<ExprAST> Parser::ParseIdentifierExpr() {
	assert(Lex.CurTok == tok_identifier && "Expected tok_identifier");
	auto Identifier = Lex.IdentifierStr;
	Lex.readNextToken(); // consume the identifier

	auto IdentifierExpr = std::make_shared<IdentifierExprAST>(Identifier);

	if (Lex.CurTok != '(') {
		// simple variable ref
		return IdentifierExpr;
	}

	// function call
	const auto Args = ParseParenExpr();
	if (!Args) {
		return nullptr;
	}

	return std::make_shared<CallExprAST>(IdentifierExpr, Args);
}

/// ifexpr ::= 'if' expression 'then' expression 'else' expression
const std::shared_ptr<IfExprAST> Parser::ParseIfExpr() {
	assert(Lex.CurTok == tok_if && "Expected tok_if");
	Lex.readNextToken(); // eat the if.

	while (Lex.CurTok == '\n') {
		Lex.readNextToken();
	}

	// condition.
	if (Lex.CurTok != '(') {
		return LogError("Expected (");
	}
	auto Cond = ParseParenExpr();
	if (!Cond) {
		return nullptr;
	}

	while (Lex.CurTok == '\n') {
		Lex.readNextToken();
	}

	if (Lex.CurTok != '{') {
		return LogError("Expected {");
	}
	auto Then = ParseCurlyExpr();
	if (!Then) {
		return nullptr;
	}

	while (Lex.CurTok == '\n') {
		Lex.readNextToken();
	}

	if (Lex.CurTok != tok_else) {
		return LogError("expected else");
	}
	Lex.readNextToken();

	while (Lex.CurTok == '\n') {
		Lex.readNextToken();
	}

	if (Lex.CurTok != '{') {
		return LogError("Expected {");
	}
	auto Else = ParseCurlyExpr();
	if (!Else) {
		return nullptr;
	}

	return std::make_shared<IfExprAST>(Cond, Then, Else);
}

/// forexpr ::= 'for' identifier '=' expr ',' expr (',' expr)? 'in' expression
const std::shared_ptr<ForExprAST> Parser::ParseForExpr() {
	assert(Lex.CurTok == tok_for && "Expected tok_for");
	Lex.readNextToken(); // eat the for.

	while (Lex.CurTok == '\n') {
		Lex.readNextToken();
	}

	if (Lex.CurTok != '(') {
		return LogError("Expected (");
	}
	auto Parts = ParseParenExpr();
	if (!Parts) {
		return nullptr;
	}

	auto Exprs = Parts->getExprs();
	if (Exprs.size() == 3) {

		while (Lex.CurTok == '\n') {
			Lex.readNextToken();
		}

		if (Lex.CurTok != '{') {
			return LogError("Expected {");
		}
		auto Body = ParseCurlyExpr();
		if (!Body) {
			return nullptr;
		}

		auto Start = Exprs[2];
		auto End = Exprs[1];
		auto Step = Exprs[0];

		return std::make_shared<ForExprAST>(Start, End, Step, Body);
	}
	else {
		return LogError("expected 3 parts");
	}
}

/// forexpr ::= 'for' identifier '=' expr ',' expr (',' expr)? 'in' expression
const std::shared_ptr<WhileExprAST> Parser::ParseWhileExpr() {
	assert(Lex.CurTok == tok_while && "Expected tok_while");
	Lex.readNextToken(); // eat the while.

	while (Lex.CurTok == '\n') {
		Lex.readNextToken();
	}

	if (Lex.CurTok != '(') {
		return LogError("Expected (");
	}
	auto Parts = ParseParenExpr();
	if (!Parts) {
		return nullptr;
	}

	while (Lex.CurTok == '\n') {
		Lex.readNextToken();
	}

	if (Lex.CurTok != '{') {
		return LogError("Expected {");
	}
	auto Body = ParseCurlyExpr();
	if (!Body) {
		return nullptr;
	}

	auto Exprs = Parts->getExprs();
	if (Exprs.size() == 1) {
		auto Test = Exprs[0];
		return std::make_shared<WhileExprAST>(Test, Body);
	}
	else {
		return LogError("Expected 1 part");
	}
}

/// varexpr ::= 'var' identifier ('=' expression)?
//                    (',' identifier ('=' expression)?)* 'in' expression
const std::shared_ptr<VarExprAST> Parser::ParseVarExpr() {
	assert(Lex.CurTok == tok_var && "Expected tok_var");
	Lex.readNextToken(); // eat the var

	while (Lex.CurTok == '\n') {
		Lex.readNextToken();
	}

	if (Lex.CurTok != '(') {
		return LogError("Expected (");
	}
	auto Idents = ParseParenExpr();
	if (!Idents) {
		return nullptr;
	}

	while (Lex.CurTok == '\n') {
		Lex.readNextToken();
	}

	if (Lex.CurTok != '=') {
		return LogError("expected =");
	}
	Lex.readNextToken(); // eat the =

	while (Lex.CurTok == '\n') {
		Lex.readNextToken();
	}

	if (Lex.CurTok != '(') {
		return LogError("Expected (");
	}
	auto Vals = ParseParenExpr();
	if (!Vals) {
		return nullptr;
	}

	while (Lex.CurTok == '\n') {
		Lex.readNextToken();
	}

	if (Lex.CurTok != '{') {
		return LogError("Expected {");
	}
	auto Body = ParseCurlyExpr();
	if (!Body) {
		return nullptr;
	}

	std::vector<std::shared_ptr<IdentifierExprAST>> Identifiers;
	for (auto Ident : Idents->getExprs()) {
		if (auto Var = llvm::dyn_cast<IdentifierExprAST>(Ident.get())) {
			Identifiers.push_back(std::make_shared<IdentifierExprAST>(*Var));
		}
		else {
			return LogError("Identifier expected in variable list");
		}
	}
	auto Values = Vals->getExprs();
	return std::make_shared<VarExprAST>(Identifiers, Values, Body);
}

const std::shared_ptr<ExprAST> Parser::ParsePrimary() {
	switch (Lex.CurTok) {
	case tok_identifier:
		return ParseIdentifierExpr();
	case tok_rationalword:
		return ParseRationalWordExpr();
	case tok_finiteword:
		return ParseFiniteWordExpr();
	case '(':
		return ParseParenExpr();
	case '{':
		return ParseCurlyExpr();
	case tok_if:
		return ParseIfExpr();
	case tok_for:
		return ParseForExpr();
	case tok_while:
		return ParseWhileExpr();
	case tok_var:
		return ParseVarExpr();
	case tok_error:
		return nullptr;
	default:
		llvm_unreachable("Unexpected token");
	}
}

/// unary
///   ::= primary
///   ::= '!' unary
const std::shared_ptr<ExprAST> Parser::ParseUnary() {
	switch (Lex.CurTok) {
		// If the current token is not an operator, it must be a primary expr.
	case tok_identifier:
	case tok_rationalword:
	case tok_finiteword:
	case tok_if:
	case tok_for:
	case tok_while:
	case tok_var:
	case '(':
	case '{': {
		auto Res = ParsePrimary();
		return Res;
	}
	case '-':
	case tok_ddd:
	case '~': {
		// If this is a unary operator, read it.
		auto Opc = Lex.CurTok;
		Lex.readNextToken();
		auto Operand = ParseUnary();
		if (!Operand) {
			return nullptr;
		}
		return std::make_shared<UnaryExprAST>(Opc, Operand);
	}
	case tok_eof:
		return LogError("Unexpected EOF");
	case '\n':
		return LogError("Unexpected newline");
	case '#':
	case '+':
	case '*':
		return LogError(stringFromToken(Lex.CurTok) + " is an infix operator");
	case '&':
	case '|':
	case '^':
	case tok_gtgt:
	case tok_gtp:
	case tok_pp:
		return LogError(stringFromToken(Lex.CurTok) + " is a binary operator");
	case tok_error:
		return nullptr;
	default:
		if (Lex.CurTok < 0) {
			llvm_unreachable("Add token");
		}
		return LogError("Unrecognized token: " + stringFromToken(Lex.CurTok));
	}
}

const std::shared_ptr<ExprAST> Parser::ParseBinOpRHS(int ExprPrec, std::shared_ptr<ExprAST> LHS) {
	// If this is a binop, find its precedence.
	while (1) {
		if (Lex.CurTok == tok_error) {
			return nullptr;
		}
		int TokPrec = GetTokPrecedence();
		if (TokPrec == -1) {
			return LogError("bad operator: " + Lex.currentState());
		}
		// If this is a binop that binds at least as tightly as the current binop,
		// consume it, otherwise we are done.
		if (TokPrec < ExprPrec) {
			return LHS;
		}

		// Okay, we know this is a binop.
		auto BinOp = Lex.CurTok;
		Lex.readNextToken(); // eat binop

		while (Lex.CurTok == '\n') {
			Lex.readNextToken();
		}

        // Parse the primary expression after the binary operator.
		auto RHS = ParseUnary();
		if (!RHS) {
			return nullptr;
		}

		// If BinOp binds less tightly with RHS than the operator after RHS, let
		// the pending operator take RHS as its LHS.
		if (Lex.CurTok == tok_error) {
			return nullptr;
		}
		int NextPrec = GetTokPrecedence();
		if (NextPrec == -1) {
			return LogError("bad operator: " + Lex.currentState());
		}
		if (TokPrec < NextPrec) {
			RHS = ParseBinOpRHS(TokPrec + 1, RHS);
			if (!RHS) {
				return nullptr;
			}
		}

		// Merge LHS/RHS.
		switch (BinOp) {
		//case ',':
		case '#':
		case '+':
		case '*':
		case tok_eqeq:
		case tok_beq:
		case '&':
		case '|':
		case '^': {
			std::vector<std::shared_ptr<ExprAST>> Args;
			Args.push_back(RHS);
			if (auto SameOpLHS = llvm::dyn_cast<InfixExprAST>(LHS.get())) {
				if (SameOpLHS->getOp() == BinOp) {
					auto SameOpLHSArgs = SameOpLHS->getArgs();
					for (auto& Arg : SameOpLHSArgs) {
						Args.push_back(Arg);
					}
				}
				else {
					Args.push_back(LHS);
				}
			}
			else {
				Args.push_back(LHS);
			}

			LHS = std::make_shared<InfixExprAST>(BinOp, Args);
			break;
		}
		default:
			LHS = std::make_shared<BinaryExprAST>(BinOp, LHS, RHS);
			break;
		}
	}
}

/// expression
///   ::= unary binoprhs
///
const std::shared_ptr<ExprAST> Parser::ParseExpression() {
	auto LHS = ParseUnary();
	if (!LHS) {
		return nullptr;
	}

	auto BinOpRHS = ParseBinOpRHS(0, LHS);
	if (!BinOpRHS) {
		return nullptr;
	}

	return BinOpRHS;
}

/// prototype
const std::shared_ptr<PrototypeAST> Parser::ParsePrototype() {
	assert(Lex.CurTok == tok_identifier && "Expected tok_identifier");
	auto Identifier = Lex.IdentifierStr;
	Lex.readNextToken(); // consume the identifier

	auto IdentifierExpr = std::make_shared<IdentifierExprAST>(Identifier);

	if (Lex.CurTok != '(') {
		return LogError("Expected (");
	}
	auto Params = ParseParenExpr();
	if (!Params) {
		return nullptr;
	}

	std::vector<std::shared_ptr<IdentifierExprAST>> Parameters;
	for (auto& Param : Params->getExprs()) {
		if (auto P = llvm::dyn_cast<IdentifierExprAST>(Param.get())) {
			Parameters.push_back(std::make_shared<IdentifierExprAST>(*P));
		}
		else {
			return LogError("Expected identifier in prototype");
		}
	}

	return std::make_shared<PrototypeAST>(IdentifierExpr, Parameters);
}

/// definition ::= 'def' prototype expression
const std::shared_ptr<DefinitionAST> Parser::ParseDefinition() {
	assert(Lex.CurTok == tok_define && "Expected tok_define");
	Lex.readNextToken(); // eat def.

	auto Proto = ParsePrototype();
	if (!Proto) {
		return nullptr;
	}

	if (Lex.CurTok != '{') {
		return LogError("Expected { in function definition");
	}
	auto E = ParseCurlyExpr();
	if (!E) {
		return nullptr;
	}

	if (Lex.CurTok != '\n' && Lex.CurTok != EOF) {
		return LogError("Did not consume all input");
	}

	return std::make_shared<DefinitionAST>(Proto, E);
}

const std::shared_ptr<ExprAST> Parser::ParseTopLevelExpression() {
	auto Expr = ParseExpression();
	if (!Expr) {
		return nullptr;
	}

	if (Lex.CurTok != '\n' && Lex.CurTok != EOF) {
		return LogError("Did not consume all input");
	}

	return Expr;
}

void Parser::throwAwayLine() {
	Lex.throwAwayLine();
}

void Parser::readNextToken() {
	Lex.readNextToken();
}

const char Parser::getCurrentToken() {
	return Lex.CurTok;
}

const std::string ForExprAST::string() const {
    return "ForExprAST";
}

const std::string VarExprAST::string() const {
    return "VarExprAST";
}

const std::string CallExprAST::string() const {
    return "CallExprAST";
}

const std::string InfixExprAST::string() const {
    return "InfixExprAST";
}

const std::string WhileExprAST::string() const {
    return "WhileExprAST";
}

const std::string UnaryExprAST::string() const {
    return "UnaryExprAST";
}

const std::string BinaryExprAST::string() const {
    return "BinaryExprAST";
}

const std::string FiniteWordExprAST::string() const {
	return Val.string();
}

const std::string RationalWordExprAST::string() const {
	return Val.string();
}

const std::string IdentifierExprAST::string() const {
    return "IdentifierExprAST";
}

const std::string ExpressionListExprAST::string() const {
    return "ExpressionListExprAST";
}

const std::string ExpressionSequenceExprAST::string() const {
    return "ExpressionSequenceExprAST";
}

const std::string IfExprAST::string() const {
    return "IfExprAST";
}

const std::string PrototypeAST::string() const {
	return "PrototypeAST";
}

const std::string DefinitionAST::string() const {
	return "DefinitionAST";
}
