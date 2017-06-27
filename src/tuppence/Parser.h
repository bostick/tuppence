//===------ Parser.h ------------------------------------------------------===//
//
// The Tuppence Programming Language
//
// This file is distributed under the MIT Open Source License.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

#pragma once

#include "Lexer.h"
#include "FiniteWord.h"
#include "RationalWord.h"
#include "Value.h"

#include <map>
#include <string>
#include <vector>

namespace tuppence {

	class AST {
	public:
		/// Discriminator for LLVM-style RTTI (dyn_cast<> et al.)
		enum ASTKind {
			AK_IdentifierAST,
			AK_PrototypeAST,
			AK_DefinitionAST,
			AK_ExpressionListExpr,
			AK_ExpressionSequenceExpr,
			AK_RationalWordExpr,
			AK_FiniteWordExpr,
			AK_IdentifierExpr,
			AK_UnaryExpr,
			AK_BinaryExpr,
			AK_InfixExpr,
			AK_CallExpr,
			AK_IfExpr,
			AK_ForExpr,
			AK_WhileExpr,
			AK_VarExpr
		};
	private:
		const ASTKind Kind;
	public:
		AST(ASTKind K) : Kind(K) {}

		virtual ~AST() {}

		virtual const std::string string() const = 0;

		const ASTKind getKind() const { return Kind; }
	};

	/// ExprAST - Base class for all expression nodes.
	class ExprAST : public AST {

	public:
		ExprAST(ASTKind Kind) : AST(Kind) {}

		virtual const std::shared_ptr<Value> eval() const = 0;
	};

	// (expr, expr, expr...)
	class ExpressionListExprAST : public ExprAST {
		const std::vector<std::shared_ptr<ExprAST>> Exprs;

	public:
		ExpressionListExprAST(std::vector<std::shared_ptr<ExprAST>> Exprs) :
			ExprAST(AK_ExpressionListExpr), Exprs(Exprs) {}

		const std::shared_ptr<Value> eval() const override;

		const std::vector<std::shared_ptr<ExprAST>> getExprs() const { return Exprs; }

		const std::string string() const override;

		static bool classof(const AST *S) {
			return S->getKind() == AK_ExpressionListExpr;
		}
	};

	// expressions separated by \n
	class ExpressionSequenceExprAST : public ExprAST {
		const std::vector<std::shared_ptr<ExprAST>> Exprs;

	public:
		ExpressionSequenceExprAST(std::vector<std::shared_ptr<ExprAST>> Exprs) :
			ExprAST(AK_ExpressionSequenceExpr), Exprs(Exprs) {}

		const std::shared_ptr<Value> eval() const override;

		const std::vector<std::shared_ptr<ExprAST>> getExprs() const { return Exprs; }

		const std::string string() const override;

		static bool classof(const AST *S) {
			return S->getKind() == AK_ExpressionSequenceExpr;
		}
	};

	/// NumberExprAST - Expression class for numeric literals like "1.0".
	class RationalWordExprAST : public ExprAST {
		const RationalWord Val;

	public:
		RationalWordExprAST(RationalWord Val) :
			ExprAST(AK_RationalWordExpr), Val(Val) {}

		const std::shared_ptr<Value> eval() const override;

		const std::string string() const override;

		static bool classof(const AST *S) {
			return S->getKind() == AK_RationalWordExpr;
		}
	};

	class FiniteWordExprAST : public ExprAST {
		const FiniteWord Val;
	public:
		FiniteWordExprAST(FiniteWord Val) :
			ExprAST(AK_FiniteWordExpr), Val(Val) {}

		const std::shared_ptr<Value> eval() const override;

		const std::string string() const override;

		static bool classof(const AST *S) {
			return S->getKind() == AK_FiniteWordExpr;
		}
	};

	/// VariableExprAST - Expression class for referencing a variable, like "a".
	class IdentifierExprAST : public ExprAST {
		const std::string Name;

	public:
		IdentifierExprAST(std::string Name) :
            ExprAST(AK_IdentifierExpr), Name(Name) {}

		std::string getName() const { return Name; }

		const std::shared_ptr<Value> eval() const override;

		const std::string string() const override;

		static bool classof(const AST *S) {
			return S->getKind() == AK_IdentifierExpr;
		}
	};

	/// UnaryExprAST - Expression class for a unary operator.
	class UnaryExprAST : public ExprAST {
		const char Opcode;
		const std::shared_ptr<ExprAST> Operand;

	public:
		UnaryExprAST(char Opcode, const std::shared_ptr<ExprAST> Operand) :
			ExprAST(AK_UnaryExpr), Opcode(Opcode), Operand(Operand) {}

		const std::shared_ptr<Value> eval() const override;

		const std::string string() const override;
	};

	/// BinaryExprAST - Expression class for a binary operator.
	class BinaryExprAST : public ExprAST {
		const char Op;
		const std::shared_ptr<ExprAST> LHS;
		const std::shared_ptr<ExprAST> RHS;

	public:
		BinaryExprAST(char Op, const std::shared_ptr<ExprAST> LHS, const std::shared_ptr<ExprAST> RHS) :
			ExprAST(AK_BinaryExpr), Op(Op), LHS(LHS), RHS(RHS) {}

        char getOp() const { return Op; }
        
		const std::shared_ptr<ExprAST> getLHS() const {
			return LHS;
		}

		const std::shared_ptr<ExprAST> getRHS() const {
			return RHS;
		}

		const std::shared_ptr<Value> eval() const override;

		const std::string string() const override;

		static bool classof(const AST *S) {
			return S->getKind() == AK_BinaryExpr;
		}
	};

	/// InfixExprAST - Expression class for a binary operator.
	class InfixExprAST : public ExprAST {
		const char Op;
		const std::vector<std::shared_ptr<ExprAST>> Args;

	public:
		InfixExprAST(char Op, std::vector<std::shared_ptr<ExprAST>> Args) :
			ExprAST(AK_InfixExpr), Op(Op), Args(Args) {}

		char getOp() const { return Op; }

		const std::vector<std::shared_ptr<ExprAST>> getArgs() const { return Args; };

		const std::shared_ptr<Value> eval() const override;

		const std::string string() const override;

		static bool classof(const AST *S) {
			return S->getKind() == AK_InfixExpr;
		}
	};

	/// CallExprAST - Expression class for function calls.
	class CallExprAST : public ExprAST {
		const std::shared_ptr<IdentifierExprAST> Callee;
		const std::shared_ptr<ExpressionListExprAST> Args;

	public:
		CallExprAST(std::shared_ptr<IdentifierExprAST> Callee, const std::shared_ptr<ExpressionListExprAST> Args) :
			ExprAST(AK_CallExpr), Callee(Callee), Args(Args) {}

		const std::shared_ptr<Value> eval() const override;

		const std::shared_ptr<ExpressionListExprAST> getArgs() const { return Args; }

		const std::string string() const override;

		static bool classof(const AST *S) {
			return S->getKind() == AK_CallExpr;
		}
	};

	/// IfExprAST - Expression class for if/then/else.
	class IfExprAST : public ExprAST {
		const std::shared_ptr<ExprAST> Cond;
		const std::shared_ptr<ExpressionSequenceExprAST> Then;
		const std::shared_ptr<ExpressionSequenceExprAST> Else;

	public:
		IfExprAST(
			const std::shared_ptr<ExprAST> Cond,
			const std::shared_ptr<ExpressionSequenceExprAST> Then,
			const std::shared_ptr<ExpressionSequenceExprAST> Else) :
			ExprAST(AK_IfExpr), Cond(Cond), Then(Then), Else(Else) {}

		const std::shared_ptr<Value> eval() const override;

		const std::string string() const override;

		static bool classof(const AST *S) {
			return S->getKind() == AK_IfExpr;
		}
	};

	/// ForExprAST - Expression class for for/in.
	class ForExprAST : public ExprAST {
		const std::shared_ptr<ExprAST> Start;
		const std::shared_ptr<ExprAST> End;
		const std::shared_ptr<ExprAST> Step;
		const std::shared_ptr<ExpressionSequenceExprAST> Body;

	public:
		ForExprAST(
			const std::shared_ptr<ExprAST> Start,
			const std::shared_ptr<ExprAST> End,
			const std::shared_ptr<ExprAST> Step,
			const std::shared_ptr<ExpressionSequenceExprAST> Body) :
			ExprAST(AK_ForExpr), Start(Start), End(End), Step(Step), Body(Body) {}

		const std::shared_ptr<Value> eval() const override;

		const std::string string() const override;

		static bool classof(const AST *S) {
			return S->getKind() == AK_ForExpr;
		}
	};

	/// ForExprAST - Expression class for for/in.
	class WhileExprAST : public ExprAST {
		const std::shared_ptr<ExprAST> Test;
		const std::shared_ptr<ExpressionSequenceExprAST> Body;

	public:
		WhileExprAST(std::shared_ptr<ExprAST> Test, std::shared_ptr<ExpressionSequenceExprAST> Body) :
			ExprAST(AK_WhileExpr), Test(Test), Body(Body) {}

		const std::shared_ptr<Value> eval() const override;

		const std::string string() const override;

		static bool classof(const AST *S) {
			return S->getKind() == AK_WhileExpr;
		}
	};

	/// VarExprAST - Expression class for var/in
	class VarExprAST : public ExprAST {
		const std::vector<std::shared_ptr<IdentifierExprAST>> Vars;
		const std::vector<std::shared_ptr<ExprAST>> Vals;
		const std::shared_ptr<ExpressionSequenceExprAST> Body;

	public:
		VarExprAST(
			std::vector<std::shared_ptr<IdentifierExprAST>> Vars,
			std::vector<std::shared_ptr<ExprAST>> Vals,
			const std::shared_ptr<ExpressionSequenceExprAST> Body) :
			ExprAST(AK_VarExpr), Vars(Vars), Vals(Vals), Body(Body) {}

		const std::shared_ptr<Value> eval() const override;

		const std::string string() const override;

		const std::vector<std::shared_ptr<IdentifierExprAST>> getVars() const { return Vars; }

		const std::vector<std::shared_ptr<ExprAST>> getVals() const { return Vals; }

		const std::shared_ptr<ExpressionSequenceExprAST> getBody() const { return Body; }

		static bool classof(const AST *S) {
			return S->getKind() == AK_VarExpr;
		}
	};

	/// PrototypeAST - This class represents the "prototype" for a function,
	/// which captures its name, and its argument names (thus implicitly the number
	/// of arguments the function takes), as well as if it is an operator.
	class PrototypeAST : public AST {
		const std::shared_ptr<IdentifierExprAST> Name;
		const std::vector<std::shared_ptr<IdentifierExprAST>> Params;

	public:
		PrototypeAST(const std::shared_ptr<IdentifierExprAST> Name, std::vector<std::shared_ptr<IdentifierExprAST>> Params) :
			AST(AK_PrototypeAST), Name(Name), Params(Params) {}

		const std::shared_ptr<IdentifierExprAST> getName() const { return Name; }

		const std::vector<std::shared_ptr<IdentifierExprAST>> getParams() const { return Params; }

		const std::string string() const override;

		static bool classof(const AST *S) {
			return S->getKind() == AK_PrototypeAST;
		}
	};

	/// FunctionAST - This class represents a function definition itself.
	class DefinitionAST : public ExprAST {
		const std::shared_ptr<PrototypeAST> Proto;
		const std::shared_ptr<ExpressionSequenceExprAST> Body;

	public:
		DefinitionAST(const std::shared_ptr<PrototypeAST> Proto, const std::shared_ptr<ExpressionSequenceExprAST> Body) :
			ExprAST(AK_DefinitionAST), Proto(Proto), Body(Body) {}

		const std::shared_ptr<Value> eval() const override;

		const std::string string() const override;

		static bool classof(const AST *S) {
			return S->getKind() == AK_DefinitionAST;
		}
	};

	class Parser {
		/// BinopPrecedence - This holds the precedence for each binary operator that is
		/// defined.
		std::map<char, int> BinopPrecedence;
		Lexer Lex;

		int GetTokPrecedence();

		/// numberexpr ::= number
		const std::shared_ptr<RationalWordExprAST> ParseRationalWordExpr();

		const std::shared_ptr<FiniteWordExprAST> ParseFiniteWordExpr();

		/// parenexpr ::= '(' expression ')'
		const std::shared_ptr<ExpressionListExprAST> ParseParenExpr();

		const std::shared_ptr<ExpressionSequenceExprAST> ParseCurlyExpr();

		/// identifierexpr
		///   ::= identifier
		///   ::= identifier '(' expression* ')'
		const std::shared_ptr<ExprAST> ParseIdentifierExpr();

		/// ifexpr ::= 'if' expression 'then' expression 'else' expression
		const std::shared_ptr<IfExprAST> ParseIfExpr();

		/// forexpr ::= 'for' identifier '=' expr ',' expr (',' expr)? 'in' expression
		const std::shared_ptr<ForExprAST> ParseForExpr();

		const std::shared_ptr<WhileExprAST> ParseWhileExpr();

		/// varexpr ::= 'var' identifier ('=' expression)?
		//                    (',' identifier ('=' expression)?)* 'in' expression
		const std::shared_ptr<VarExprAST> ParseVarExpr();

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

	public:
		Parser(std::istream &);

		/// expression
		///   ::= primary binoprhs
		///
		const std::shared_ptr<ExprAST> ParseTopLevelExpression();

		/// definition ::= 'def' prototype expression
		const std::shared_ptr<DefinitionAST> ParseDefinition();

		/// prototype
		///   ::= id '(' id* ')'
		const std::shared_ptr<PrototypeAST> ParsePrototype();

		void throwAwayLine();
		void readNextToken();
		const char getCurrentToken();

	};
}
