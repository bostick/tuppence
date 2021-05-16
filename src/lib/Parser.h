//===------ Parser.h ------------------------------------------------------===//
//
// The Tuppence Programming Language
//
// This file is distributed under the MIT Open Source License.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

#pragma once

// DO NOT INCLUDE VALUE.H here, Value.h includes Parser.h
#include "../common/Lexer.h"
#include "../common/AST.h"

#include "llvm/IR/Value.h"

#include <map>
#include <string>
#include <vector>

namespace tuppence {
    
	class Parser {
		/// BinopPrecedence - This holds the precedence for each binary operator that is
		/// defined.
		std::map<char, int> BinopPrecedence;
		Lexer Lex;

		int GetTokPrecedence();

		/// numberexpr ::= number
		const std::shared_ptr<IntegerWordExprAST> ParseIntegerWordExpr();

		const std::shared_ptr<FiniteWordExprAST> ParseFiniteWordExpr();

		/// parenexpr ::= '(' expression ')'
		const std::shared_ptr<ExprAST> ParseParenExpr();

        /// parenexpr ::= '(' expression ')'
        const std::shared_ptr<ExpressionListExprAST> ParseSingleParenExpr();
        
		const std::shared_ptr<ExpressionSequenceExprAST> ParseCurlyExpr();

		/// identifierexpr
		///   ::= identifier
		///   ::= identifier '(' expression* ')'
		const std::shared_ptr<ExprAST> ParseIdentifierExpr();

		/// ifexpr ::= 'if' expression 'then' expression 'else' expression
		const std::shared_ptr<IfExprAST> ParseIfExpr();

		/// forexpr ::= 'for' identifier '=' expr ',' expr (',' expr)? 'in' expression
//        const std::shared_ptr<ForExprAST> ParseForExpr();

		const std::shared_ptr<WhileExprAST> ParseWhileExpr();
        
		/// primary
		///   ::= identifierexpr
		///   ::= numberexpr
		///   ::= parenexpr
		const std::shared_ptr<ExprAST> ParsePrimary();

		/// unary
		///   ::= primary
		///   ::= '!' unary
		const std::shared_ptr<ExprAST> ParseUnary();

		/// binoprhs
		///   ::= ('+' primary)*
		const std::shared_ptr<ExprAST> ParseBinOpRHS(int ExprPrec, std::shared_ptr<ExprAST> LHS);

		/// expression
		///   ::= primary binoprhs
		///
		const std::shared_ptr<ExprAST> ParseExpression();
        
        /// prototype
        ///   ::= id '(' id* ')'
        const std::shared_ptr<PrototypeAST> ParsePrototype();

	public:
		Parser(std::istream &);

		/// expression
		///   ::= primary binoprhs
		///
		const std::shared_ptr<DefinitionAST> ParseTopLevelExpression();

        /// definition ::= 'define' prototype expression
        const std::shared_ptr<DefinitionAST> ParseDefinition();
        
		void throwAwayLine();
		void readNextToken();
		char getCurrentToken();

	};
    
    std::nullptr_t LogParserError(std::string Str);

} // end namespace tuppence
