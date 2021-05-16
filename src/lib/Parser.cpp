//===------ Parser.cpp ----------------------------------------------------===//
//
// The Tuppence Programming Language
//
// This file is distributed under the MIT Open Source License.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

#include "../lib/Parser.h"

#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/raw_ostream.h"

#include <sstream>

using namespace tuppence;

std::nullptr_t tuppence::LogParserError(std::string Str) {
    llvm::errs() << "Error: " << Str << "\n";
    return nullptr;
}

Parser::Parser(std::istream &In) :
	Lex(Lexer(In)) {
	// 1 is lowest precedence.
	BinopPrecedence['='] = 3;

	BinopPrecedence['|'] = 7;
	BinopPrecedence['^'] = 8;
	BinopPrecedence['&'] = 9;
	BinopPrecedence[tok_equal_equal] = 10;
	BinopPrecedence[tok_bang_equal] = 10;
    BinopPrecedence[tok_percent_percent] = 19;
	BinopPrecedence[tok_greater_greater] = 19;
	BinopPrecedence[tok_greater_percent] = 19;
    BinopPrecedence[tok_slash_percent] = 19;
	BinopPrecedence['+'] = 20;
	BinopPrecedence['-'] = 20;
        
	BinopPrecedence['*'] = 40;
	BinopPrecedence['/'] = 40;
        
	BinopPrecedence['#'] = 50;
	BinopPrecedence[tok_star_star] = 50;
        
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

const std::shared_ptr<IntegerWordExprAST> Parser::ParseIntegerWordExpr() {
	assert(Lex.CurTok == tok_integerword && "Expected tok_integerword");
	auto Result = std::make_shared<IntegerWordExprAST>(Lex.IntegerWordStr);
	Lex.readNextToken(); // consume the number
	return Result;
}

const std::shared_ptr<FiniteWordExprAST> Parser::ParseFiniteWordExpr() {
	assert(Lex.CurTok == tok_finiteword && "Expected tok_finiteword");
	auto Result = std::make_shared<FiniteWordExprAST>(Lex.FiniteWordStr);
	Lex.readNextToken(); // consume the `
	return Result;
}

const std::shared_ptr<ExpressionListExprAST> Parser::ParseSingleParenExpr() {
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
                return LogParserError(std::string("unexpected token in Paren expr: ") + stringFromToken(Lex.CurTok));
			}
		}
	}
	if (Lex.CurTok != ')') {
		return LogParserError("expected ')'");
	}
	Lex.readNextToken(); // eat ).
    
    return std::make_shared<ExpressionListExprAST>(Exprs);
}

const std::shared_ptr<ExprAST> Parser::ParseParenExpr() {
    auto Single = ParseSingleParenExpr();
    
    if (Lex.CurTok != '(') {
        // simple variable ref
        return Single;
    }
    
    // function call
    const auto Args = ParseSingleParenExpr();
    if (!Args) {
        return nullptr;
    }
    
    auto Exprs = Single->getExprs();
    if (Exprs.size() != 1) {
        return LogParserError("Expected 1 argument in parentheses");
    }
    
    auto E = Exprs[0];
    
    return std::make_shared<CallExprAST>(E, Args);
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
		return LogParserError("expected '}'");
	}
	Lex.readNextToken(); // eat }.

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
	const auto Args = ParseSingleParenExpr();
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
		return LogParserError("Expected (");
	}
	auto Cond = ParseParenExpr();
	if (!Cond) {
		return nullptr;
	}
    
	while (Lex.CurTok == '\n') {
		Lex.readNextToken();
	}

	if (Lex.CurTok != '{') {
		return LogParserError("Expected {");
	}
	auto Then = ParseCurlyExpr();
	if (!Then) {
		return nullptr;
	}

	while (Lex.CurTok == '\n') {
		Lex.readNextToken();
	}

	if (Lex.CurTok != tok_else) {
		return LogParserError("expected else");
	}
	Lex.readNextToken();

	while (Lex.CurTok == '\n') {
		Lex.readNextToken();
	}

	if (Lex.CurTok != '{') {
		return LogParserError("Expected {");
	}
	auto Else = ParseCurlyExpr();
	if (!Else) {
		return nullptr;
	}

	return std::make_shared<IfExprAST>(Cond, Then, Else);
}

/// forexpr ::= 'for' identifier '=' expr ',' expr (',' expr)? 'in' expression
//const std::shared_ptr<ForExprAST> Parser::ParseForExpr() {
//    assert(Lex.CurTok == tok_for && "Expected tok_for");
//    Lex.readNextToken(); // eat the for.
//
//    while (Lex.CurTok == '\n') {
//        Lex.readNextToken();
//    }
//
//    if (Lex.CurTok != '(') {
//        return LogParserError("Expected (");
//    }
//    auto Parts = ParseSingleParenExpr();
//    if (!Parts) {
//        return nullptr;
//    }
//
//    auto Exprs = Parts->getExprs();
//    if (Exprs.size() == 3) {
//
//        while (Lex.CurTok == '\n') {
//            Lex.readNextToken();
//        }
//
//        if (Lex.CurTok != '{') {
//            return LogParserError("Expected {");
//        }
//        auto Body = ParseCurlyExpr();
//        if (!Body) {
//            return nullptr;
//        }
//
//        auto Start = Exprs[2];
//        auto End = Exprs[1];
//        auto Step = Exprs[0];
//
//        return std::make_shared<ForExprAST>(Start, End, Step, Body);
//    }
//    else {
//        return LogParserError("expected 3 parts");
//    }
//}

/// forexpr ::= 'for' identifier '=' expr ',' expr (',' expr)? 'in' expression
const std::shared_ptr<WhileExprAST> Parser::ParseWhileExpr() {
	assert(Lex.CurTok == tok_while && "Expected tok_while");
	Lex.readNextToken(); // eat the while.

	while (Lex.CurTok == '\n') {
		Lex.readNextToken();
	}

	if (Lex.CurTok != '(') {
		return LogParserError("Expected (");
	}
	auto Parts = ParseSingleParenExpr();
	if (!Parts) {
		return nullptr;
	}

	while (Lex.CurTok == '\n') {
		Lex.readNextToken();
	}

	if (Lex.CurTok != '{') {
		return LogParserError("Expected {");
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
		return LogParserError("Expected 1 part");
	}
}

const std::shared_ptr<ExprAST> Parser::ParsePrimary() {
	switch (Lex.CurTok) {
	case tok_identifier:
		return ParseIdentifierExpr();
	case tok_integerword:
		return ParseIntegerWordExpr();
	case tok_finiteword:
		return ParseFiniteWordExpr();
	case '(':
		return ParseParenExpr();
	case '{':
		return ParseCurlyExpr();
	case tok_if:
		return ParseIfExpr();
//    case tok_for:
//        return ParseForExpr();
	case tok_while:
		return ParseWhileExpr();
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
	case tok_integerword:
	case tok_finiteword:
	case tok_if:
//    case tok_for:
	case tok_while:
	case '(':
	case '{': {
		auto Res = ParsePrimary();
		return Res;
	}
	case '-':
	case tok_dot_dot_dot:
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
		return LogParserError("Unexpected EOF");
	case '\n':
		return LogParserError("Unexpected newline");
	case '#':
	case '+':
	case '*':
            return LogParserError(std::string("") + stringFromToken(Lex.CurTok) + " is an infix operator");
	case '&':
	case '|':
	case '^':
	case tok_greater_greater:
	case tok_greater_percent:
	case tok_percent_percent:
    case tok_star_star:
    case tok_slash_percent:
		return LogParserError(std::string("") + stringFromToken(Lex.CurTok) + " is a binary operator");
	case tok_error:
		return nullptr;
	default:
		if (Lex.CurTok < 0) {
			llvm_unreachable("Add this token");
		}
            return LogParserError(std::string("Unrecognized token: ") + stringFromToken(Lex.CurTok));
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
			return LogParserError("Unrecognized operator: " + Lex.currentState());
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
			return LogParserError("Unrecognized operator: " + Lex.currentState());
		}
		if (TokPrec < NextPrec) {
			RHS = ParseBinOpRHS(TokPrec + 1, RHS);
			if (!RHS) {
				return nullptr;
			}
		}

		// Merge LHS/RHS.
		switch (BinOp) {
		case '#':
		case '+':
		case '*':
		case tok_equal_equal:
		case tok_bang_equal:
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
        return LogParserError("Expected (");
    }
    auto Params = ParseSingleParenExpr();
    if (!Params) {
        return nullptr;
    }
    
    std::vector<std::shared_ptr<ExprAST>> Parameters;
    auto Exprs = Params->getExprs();
    
    if (!Exprs.empty()) {
        auto Iter = Exprs.begin();
        for (; Iter != --Exprs.end(); Iter++) {
            auto Param = *Iter;
            if (auto P = llvm::dyn_cast<IdentifierExprAST>(Param.get())) {
                Parameters.push_back(Param);
            }
            else {
                return LogParserError("Expected identifier in prototype");
            }
        }
        
        auto Last = *Iter;
        if (auto P = llvm::dyn_cast<IdentifierExprAST>(Last.get())) {
            Parameters.push_back(Last);
        }
        else if (auto P = llvm::dyn_cast<UnaryExprAST>(Last.get())) {
            if (P->getOperator() == tok_dot_dot_dot) {
                Parameters.push_back(Last);
            } else {
                return LogParserError("Expected ...identifier in prototype");
            }
        }
        else {
            return LogParserError("Expected identifier or ...identifier in prototype");
        }
    }
    
    return std::make_shared<PrototypeAST>(IdentifierExpr, std::make_shared<ExpressionListExprAST>(Parameters));
}

/// definition ::= 'define' prototype expression
const std::shared_ptr<DefinitionAST> Parser::ParseDefinition() {
    assert(Lex.CurTok == tok_define && "Expected tok_define");
    Lex.readNextToken(); // eat define
    
    if (Lex.CurTok != tok_identifier) {
        return LogParserError("Expected identifier");
    }
    auto Proto = ParsePrototype();
    if (!Proto) {
        return nullptr;
    }
    
    if (Lex.CurTok != '{') {
        return LogParserError("Expected { in function definition");
    }
    auto E = ParseCurlyExpr();
    if (!E) {
        return nullptr;
    }
    
    if (Lex.CurTok != '\n' && Lex.CurTok != EOF) {
        return LogParserError("Did not consume all input");
    }
    
    return std::make_shared<DefinitionAST>(Proto, E);
}

const std::shared_ptr<DefinitionAST> Parser::ParseTopLevelExpression() {
    
    auto IdentifierExpr = std::make_shared<IdentifierExprAST>("__anon_expr");
    
    std::vector<std::shared_ptr<ExprAST>> Parameters;
    auto ParameterList = std::make_shared<ExpressionListExprAST>(Parameters);
    auto Proto = std::make_shared<PrototypeAST>(IdentifierExpr, ParameterList);
    
    auto Body = ParseExpression();
    if (!Body) {
        return nullptr;
    }
    
    if (Lex.CurTok != '\n' && Lex.CurTok != EOF) {
        return LogParserError("Did not consume all input");
    }
    
    return std::make_shared<DefinitionAST>(Proto, Body);
}

void Parser::throwAwayLine() {
	Lex.throwAwayLine();
}

void Parser::readNextToken() {
	Lex.readNextToken();
}

char Parser::getCurrentToken() {
	return Lex.CurTok;
}
