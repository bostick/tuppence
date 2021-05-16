//===------ AST.h ------------------------------------------------------===//
//
// The Tuppence Programming Language
//
// This file is distributed under the MIT Open Source License.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

#pragma once

// DO NOT INCLUDE VALUE.H here, Value.h includes Parser.h
// #include "../common/Lexer.h"

#include "llvm/IR/Value.h"

#include <map>
#include <string>
#include <vector>

namespace tuppence {

    // Forward declare
    // struct Value;
    
    // class UserFunction;
    // class BuiltinFunction;
    
	class AST {
	public:
		/// Discriminator for LLVM-style RTTI (dyn_cast<> et al.)
		enum ASTKind {
            AK_ExternalAST,
//            AK_InternalAST,
            AK_DefinitionAST,
            AK_CommandAST,
			AK_PrototypeAST,
			AK_ExpressionListExpr,
			AK_ExpressionSequenceExpr,
			AK_IntegerWordExpr,
			AK_FiniteWordExpr,
			AK_IdentifierExpr,
			AK_UnaryExpr,
			AK_BinaryExpr,
			AK_InfixExpr,
			AK_CallExpr,
			AK_IfExpr,
			AK_ForExpr,
			AK_WhileExpr
		};
	private:
		const ASTKind Kind;
	public:
		AST(ASTKind K) : Kind(K) {}

		virtual ~AST() {}

		virtual const std::string string() const = 0;
        
        virtual const std::string typesetString() const;
        
		ASTKind getKind() const { return Kind; }
	};
    
// 	/// ExprAST - Base class for all expression nodes.
	class ExprAST : public AST {

	public:
		ExprAST(ASTKind Kind) : AST(Kind) {}

//        virtual const std::shared_ptr<Value> eval() const = 0;
        
        virtual llvm::Value *codegen() = 0;
	};

	// (expr, expr, expr...)
	class ExpressionListExprAST : public ExprAST {
		const std::vector<std::shared_ptr<ExprAST>> Exprs;

	public:
		ExpressionListExprAST(std::vector<std::shared_ptr<ExprAST>> Exprs) :
			ExprAST(AK_ExpressionListExpr), Exprs(Exprs) {}

        typedef std::vector<std::shared_ptr<ExprAST>>::const_iterator const_iterator;
        typedef std::vector<std::shared_ptr<ExprAST>>::const_reverse_iterator const_reverse_iterator;
        
        const_iterator begin() const { return Exprs.begin(); }
        const_iterator end() const { return Exprs.end(); }
        
        const_reverse_iterator rbegin() const { return Exprs.rbegin(); }
        const_reverse_iterator rend() const { return Exprs.rend(); }
        
        bool empty() const { return Exprs.empty(); }
        
        const std::shared_ptr<ExprAST> get(size_t i) { return Exprs[i]; }
        
        size_t size() const { return Exprs.size(); }
        
//        const std::shared_ptr<Value> eval() const override;

        llvm::Value *codegen() override;
        
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

 //        const std::shared_ptr<Value> eval() const override;

         llvm::Value *codegen() override;
        
         const std::vector<std::shared_ptr<ExprAST>> getExprs() const { return Exprs; }

         const std::string string() const override;

         const std::string typesetString() const override;
        
         static bool classof(const AST *S) {
             return S->getKind() == AK_ExpressionSequenceExpr;
         }
     };

	/// NumberExprAST - Expression class for numeric literals like "1.0".
     class IntegerWordExprAST : public ExprAST {
         const std::string Val;

     public:
         IntegerWordExprAST(std::string Val) :
             ExprAST(AK_IntegerWordExpr), Val(Val) {}

 //        const std::shared_ptr<Value> eval() const override;

         llvm::Value *codegen() override;
        
         const std::string string() const override;

         static bool classof(const AST *S) {
             return S->getKind() == AK_IntegerWordExpr;
         }
     };

     class FiniteWordExprAST : public ExprAST {
         const std::string Val;
     public:
         FiniteWordExprAST(std::string Val) :
             ExprAST(AK_FiniteWordExpr), Val(Val) {}

 //        const std::shared_ptr<Value> eval() const override;

         llvm::Value *codegen() override;
        
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

//        const std::shared_ptr<Value> eval() const override;

        llvm::Value *codegen() override;
        
		const std::string string() const override;

		static bool classof(const AST *S) {
			return S->getKind() == AK_IdentifierExpr;
		}
	};

	/// UnaryExprAST - Expression class for a unary operator.
     class UnaryExprAST : public ExprAST {
         const char Operator;
         const std::shared_ptr<ExprAST> Operand;

     public:
         UnaryExprAST(char Operator, const std::shared_ptr<ExprAST> Operand) :
             ExprAST(AK_UnaryExpr), Operator(Operator), Operand(Operand) {}

         char getOperator() const { return Operator; }
        
         const std::shared_ptr<ExprAST> getOperand() const { return Operand; }
        
 //        const std::shared_ptr<Value> eval() const override;

         llvm::Value *codegen() override;
        
         const std::string string() const override;
        
         static bool classof(const AST *S) {
             return S->getKind() == AK_UnaryExpr;
         }
     };

	/// BinaryExprAST - Expression class for a binary operator.
     class BinaryExprAST : public ExprAST {
         const char Op;
         const std::shared_ptr<ExprAST> LHS;
         const std::shared_ptr<ExprAST> RHS;

     public:
         BinaryExprAST(char Op, const std::shared_ptr<ExprAST> LHS, const std::shared_ptr<ExprAST> RHS) :
             ExprAST(AK_BinaryExpr), Op(Op), LHS(LHS), RHS(RHS) {}

         char getOperator() const { return Op; }
        
         const std::shared_ptr<ExprAST> getLHS() const {
             return LHS;
         }

         const std::shared_ptr<ExprAST> getRHS() const {
             return RHS;
         }

 //        const std::shared_ptr<Value> eval() const override;

         llvm::Value *codegen() override;
        
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

 //        const std::shared_ptr<Value> eval() const override;

         llvm::Value *codegen() override;
        
         const std::string string() const override;

         static bool classof(const AST *S) {
             return S->getKind() == AK_InfixExpr;
         }
     };

	/// CallExprAST - Expression class for function calls.
     class CallExprAST : public ExprAST {
         const std::shared_ptr<ExprAST> Callee;
         const std::shared_ptr<ExpressionListExprAST> Args;

     public:
         CallExprAST(std::shared_ptr<ExprAST> Callee, const std::shared_ptr<ExpressionListExprAST> Args) :
             ExprAST(AK_CallExpr), Callee(Callee), Args(Args) {}

 //        const std::shared_ptr<Value> eval() const override;

         llvm::Value *codegen() override;
        
         const std::shared_ptr<ExprAST> getCallee() const { return Callee; }
        
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

 //        const std::shared_ptr<Value> eval() const override;

         llvm::Value *codegen() override;
        
         const std::string string() const override;

         static bool classof(const AST *S) {
             return S->getKind() == AK_IfExpr;
         }
     };

	/// ForExprAST - Expression class for for.
//     class ForExprAST : public ExprAST {
//         const std::shared_ptr<ExprAST> Start;
//         const std::shared_ptr<ExprAST> End;
//         const std::shared_ptr<ExprAST> Step;
//         const std::shared_ptr<ExpressionSequenceExprAST> Body;
//
//     public:
//         ForExprAST(
//             const std::shared_ptr<ExprAST> Start,
//             const std::shared_ptr<ExprAST> End,
//             const std::shared_ptr<ExprAST> Step,
//             const std::shared_ptr<ExpressionSequenceExprAST> Body) :
//             ExprAST(AK_ForExpr), Start(Start), End(End), Step(Step), Body(Body) {}
//
// //        const std::shared_ptr<Value> eval() const override;
//
//         llvm::Value *codegen() override;
//        
//         const std::string string() const override;
//
//         static bool classof(const AST *S) {
//             return S->getKind() == AK_ForExpr;
//         }
//     };

	/// WhileExprAST - Expression class for while.
     class WhileExprAST : public ExprAST {
         const std::shared_ptr<ExprAST> Test;
         const std::shared_ptr<ExpressionSequenceExprAST> Body;

     public:
         WhileExprAST(std::shared_ptr<ExprAST> Test, std::shared_ptr<ExpressionSequenceExprAST> Body) :
             ExprAST(AK_WhileExpr), Test(Test), Body(Body) {}

 //        const std::shared_ptr<Value> eval() const override;

         llvm::Value *codegen() override;
        
         const std::string string() const override;

         static bool classof(const AST *S) {
             return S->getKind() == AK_WhileExpr;
         }
     };

    /// PrototypeAST - This class represents the "prototype" for a function,
    /// which captures its name, and its argument names (thus implicitly the number
    /// of arguments the function takes), as well as if it is an operator.
    class PrototypeAST : public AST {
        const std::shared_ptr<IdentifierExprAST> Name;
        const std::shared_ptr<ExpressionListExprAST> Params;
        
    public:
        PrototypeAST(const std::shared_ptr<IdentifierExprAST> Name, std::shared_ptr<ExpressionListExprAST> Params) :
        AST(AK_PrototypeAST), Name(Name), Params(Params) {}
        
        const std::shared_ptr<IdentifierExprAST> getName() const { return Name; }
        
        std::shared_ptr<ExpressionListExprAST> getParams() const { return Params; }
        
        llvm::Function *codegen();
        
        // eval
        
        const std::string string() const override;
        
        // bool match(std::vector<std::shared_ptr<Value>> ArgsV) const;
        
        bool match(size_t Count) const;
        
        static bool classof(const AST *S) {
            return S->getKind() == AK_PrototypeAST;
        }
    };
    
    /// ExternalAST - This class represents an external function declaration
//     class ExternalAST : public AST {
//         const std::shared_ptr<PrototypeAST> Proto;
//
//     public:
//         ExternalAST(const std::shared_ptr<PrototypeAST> Proto) :
//         AST(AK_ExternalAST), Proto(Proto) {}
//
//         const std::shared_ptr<PrototypeAST> getProto() const { return Proto; }
//
// //        const std::shared_ptr<BuiltinFunction> declare() const;
//
//         llvm::Function *codegen();
//
//         const std::string string() const override;
//
//         static bool classof(const AST *S) {
//             return S->getKind() == AK_ExternalAST;
//         }
//     };
    
    /// InternalAST
//    class InternalAST : public AST {
//        const std::shared_ptr<PrototypeAST> Proto;
//        
//    public:
//        InternalAST(const std::shared_ptr<PrototypeAST> Proto) :
//        AST(AK_InternalAST), Proto(Proto) {}
//        
//        const std::shared_ptr<PrototypeAST> getProto() const { return Proto; }
//        
////        const std::shared_ptr<BuiltinFunction> declare() const;
//        
//        llvm::Value *codegen();
//        
//        const std::string string() const override;
//        
//        static bool classof(const AST *S) {
//            return S->getKind() == AK_InternalAST;
//        }
//    };
    
    /// DefinitionAST - This class represents a function definition itself.
     class DefinitionAST : public AST {
         const std::shared_ptr<PrototypeAST> ProtoAST;
         const std::shared_ptr<ExprAST> Body;
        
     public:
         DefinitionAST(const std::shared_ptr<PrototypeAST> ProtoAST, const std::shared_ptr<ExprAST> Body) :
         AST(AK_DefinitionAST), ProtoAST(ProtoAST), Body(Body) {}
        
         const std::shared_ptr<PrototypeAST> getProtoAST() const { return ProtoAST; }
        
         const std::shared_ptr<ExprAST> getBody() const { return Body; }
        
 //        const std::shared_ptr<UserFunction> define() const;
        
         llvm::Function *codegen();
        
         const std::string string() const override;
        
         static bool classof(const AST *S) {
             return S->getKind() == AK_DefinitionAST;
         }
     };
    
    /// CommandAST - This class represents a command
//     class CommandAST : public AST {
//         const std::string Command;
//        
//     public:
//         CommandAST(const std::string Command) :
//         AST(AK_CommandAST), Command(Command) {}
//        
//         const std::string getCommand() const { return Command; }
//        
//         const std::string string() const override;
//        
//         static bool classof(const AST *S) {
//             return S->getKind() == AK_CommandAST;
//         }
//     };
    
} // end namespace tuppence

