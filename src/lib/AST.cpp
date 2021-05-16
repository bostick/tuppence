//===------ AST.cpp ----------------------------------------------------===//
//
// The Tuppence Programming Language
//
// This file is distributed under the MIT Open Source License.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

#include "../common/AST.h"

#include "../common/Lexer.h"
// #include "tuppence/Logger.h"

#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/Casting.h"

#include <sstream>

using namespace tuppence;

// Use string() if a more specific definition is not provided
const std::string AST::typesetString() const {
    return string();
}

//const std::string ForExprAST::string() const {
//    return "ForExprAST";
//}

const std::string CallExprAST::string() const {
    return getCallee()->string() + getArgs()->string();
}

const std::string InfixExprAST::string() const {
    
    std::stringstream ss;
    auto Args = getArgs();
    auto Op = stringFromToken(getOp());
    // printing is reversed from normal, i.e., printing is done right-to-left
    auto Iter = Args.rbegin();
    for (; Iter != --Args.rend(); Iter++) {
        auto P = *Iter;
        ss << P->string();
        ss << Op;
    }
    auto Last = *Iter;
    ss << Last->string();
    
    return ss.str();
}

const std::string WhileExprAST::string() const {
    return "WhileExprAST";
}

const std::string UnaryExprAST::string() const {
    return stringFromToken(getOperator()) + getOperand()->string();
}

const std::string BinaryExprAST::string() const {
    return getLHS()->string() + stringFromToken(getOperator()) + getRHS()->string();
}

const std::string FiniteWordExprAST::string() const {
    return "`" + Val + "`";
}

const std::string IntegerWordExprAST::string() const {
    return Val;
}

const std::string IdentifierExprAST::string() const {
    return Name;
}

const std::string ExpressionListExprAST::string() const {
    std::stringstream ss;
    ss << "(";
    if (!Exprs.empty()) {
        // printing is reversed from normal, i.e., printing is done right-to-left
        auto Iter = Exprs.rbegin();
        for (; Iter != --Exprs.rend(); Iter++) {
            auto P = *Iter;
            ss << P->string();
            ss << ", ";
        }
        auto Last = *Iter;
        ss << Last->string();
    }
    
    ss << ")";
    return ss.str();
}

const std::string ExpressionSequenceExprAST::string() const {
    std::stringstream ss;
    ss << "{\n";
    auto Iter = Exprs.begin();
    for (; Iter != Exprs.end(); Iter++) {
        auto Expr = *Iter;
        ss << Expr->string();
        ss << "\n";
    }
    ss << "}";
    return ss.str();
}

const std::string ExpressionSequenceExprAST::typesetString() const {
    std::stringstream ss;
    ss << "\\unicode{123}\n";
    auto Iter = Exprs.begin();
    for (; Iter != Exprs.end(); Iter++) {
        auto Expr = *Iter;
        ss << Expr->string();
        ss << "\n";
    }
    ss << "\\unicode{125}\n";
    return ss.str();
}

const std::string IfExprAST::string() const {
    return "if (" + Cond->string() + ") " + Then->string() + " else " + Else->string() + "";
}

const std::string PrototypeAST::string() const {
    return getName()->string() + getParams()->string();
}

//bool PrototypeAST::match(std::vector<std::shared_ptr<Value>> ArgsV) const {
//    auto IterArgs = ArgsV.begin();
//    if (!Params->empty()) {
//        auto IterParam = Params->begin();
//        for (; IterParam != --Params->end(); IterParam++) {
//            auto Param = *IterParam;
//            if (auto P = llvm::dyn_cast<IdentifierExprAST>(Param.get())) {
//                if (IterArgs == ArgsV.end()) {
//                    // not enough actual params
//                    return false;
//                }
//                IterArgs++;
//            } else {
//                assert(false);
//                return false;
//            }
//        }
//        auto Last = *IterParam;
//        if (auto P = llvm::dyn_cast<IdentifierExprAST>(Last.get())) {
//            if (IterArgs == ArgsV.end()) {
//                // not enough actual params
//                return false;
//            }
//            IterArgs++;
//        }
//        else if (auto P = llvm::dyn_cast<UnaryExprAST>(Last.get())) {
//            assert(P->getOperator() == tok_dot_dot_dot);
//            // eat the rest of the actual params
//            IterArgs = ArgsV.end();
//        } else {
//            assert(false);
//            return false;
//        }
//    }
//    if (IterArgs != ArgsV.end()) {
//        // Too many actual params
//        return false;
//    }
//
//    return true;
//}

bool PrototypeAST::match(size_t ActualCount) const {
    
    auto ParamSize = Params->size();
    
    if (ParamSize == ActualCount) {
        return true;
    }
    
    return false;
}

const std::string DefinitionAST::string() const {
    return "DefinitionAST";
}

//const std::string CommandAST::string() const {
//    return "CommandAST";
//}

//const std::string ExternalAST::string() const {
//    return "ExternalAST";
//}

//const std::string InternalAST::string() const {
//    return "InternalAST";
//}

