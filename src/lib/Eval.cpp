//===------ Eval.cpp ------------------------------------------------------===//
//
// The Tuppence Programming Language
//
// This file is distributed under the MIT Open Source License.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

#include "../lib/Eval.h"

#include "../common/BuiltinFunction.h"
#include "../common/Lexer.h"
#include "../common/AST.h"

#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/Casting.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"

#include <cstdlib>

using namespace tuppence;

llvm::LLVMContext eval::TheContext;
llvm::IRBuilder<> eval::Builder(TheContext);
std::unique_ptr<llvm::Module> eval::TheModule;
std::unique_ptr<llvm::legacy::FunctionPassManager> eval::TheFPM;
std::unique_ptr<llvm::orc::KaleidoscopeJIT> eval::TheJIT;

llvm::IntegerType *eval::types::Int1Ty;
llvm::IntegerType *eval::types::Int8Ty;
llvm::IntegerType *eval::types::Int32Ty;
llvm::IntegerType *eval::types::Int64Ty;

llvm::IntegerType *eval::types::CharTy;
llvm::IntegerType *eval::types::SizeTTy;

llvm::PointerType *eval::types::VoidPtrTy;
llvm::PointerType *eval::types::CharPtrTy;
llvm::PointerType *eval::types::Int8PtrTy;

llvm::PointerType *eval::types::FiniteWordPtrTy;
llvm::PointerType *eval::types::RationalWordPtrTy;
llvm::PointerType *eval::types::ListPtrTy;
llvm::PointerType *eval::types::BuiltinSymbolPtrTy;
llvm::PointerType *eval::types::BuiltinFunctionPtrTy;

llvm::StructType *eval::types::TuppenceValueTy;
llvm::PointerType *eval::types::TuppenceValuePtrTy;
llvm::PointerType *eval::types::TuppenceValuePtrPtrTy;

std::map<std::string, llvm::FunctionType *> eval::RuntimeFunctionTypes;

std::map<std::string, TuppenceValue**> eval::NamedValues;

llvm::Function *getRuntimeFunction(std::string Name);

llvm::Constant* AddGlobalString(std::string S, std::string Name);
llvm::Value *LogError(std::string ErrStr);

llvm::GlobalVariable *createGlobal(std::string Name, TuppenceValue **V);
    
    




void eval::InitializeModuleAndPassManager() {
    // Open a new module.
    eval::TheModule = llvm::make_unique<llvm::Module>("my cool jit", eval::TheContext);
    
    eval::TheModule->setDataLayout(eval::TheJIT->getTargetMachine().createDataLayout());
    
    // Create a new pass manager attached to it.
    eval::TheFPM = llvm::make_unique<llvm::legacy::FunctionPassManager>(eval::TheModule.get());
    
    // Do simple "peephole" optimizations and bit-twiddling optzns.
//    eval::TheFPM->add(llvm::createInstructionCombiningPass());
    // Reassociate expressions.
//    eval::TheFPM->add(llvm::createReassociatePass());
    // Eliminate Common SubExpressions.
//    eval::TheFPM->add(llvm::createGVNPass());
    // Simplify the control flow graph (deleting unreachable blocks, etc).
//    eval::TheFPM->add(llvm::createCFGSimplificationPass());
    
    eval::TheFPM->doInitialization();
    
}

//llvm::Value *getFunction(std::string Name) {
//
//    // If not, check whether we can codegen the declaration from some existing
//    // prototype.
//    auto FI = eval::FunctionPrototypes.find(Name);
//    if (FI != eval::FunctionPrototypes.end()) {
//
//        auto Proto = FI->second;
//
//        return Proto->codegen();
//    }
//
//    // If no existing prototype exists, return null.
//    return nullptr;
//}


llvm::Function *PrototypeAST::codegen() {

    // First, see if the function has already been added to the current module.
    if (auto *F = eval::TheModule->getFunction(Name->getName())) {
        return F;
    }

//    auto Params = ProtoAST->getParams();
    
//    auto ParamCount = Params->size();
    std::vector<llvm::Type *> ParamTys;
    ParamTys.push_back(eval::types::TuppenceValuePtrPtrTy);
    ParamTys.push_back(eval::types::SizeTTy);

    //    auto RetTy = llvm::Type::getIntNTy(eval::TheContext, 4);
    auto RetTy = eval::types::TuppenceValuePtrTy;
    llvm::FunctionType *FT = llvm::FunctionType::get(RetTy, ParamTys, false);

    llvm::Function *F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, Name->getName(), eval::TheModule.get());

    // Set names for all arguments.
//    unsigned Idx = 0;
//    for (auto &Arg : F->args()) {
//        // Setup names in reverse order, so that function definitions look good, e.g.:
//        // define i4 @foo(i4 %a, i4 %b, i4 %c, i4 %d)
//        // and not:
//        // define i4 @foo(i4 %d, i4 %c, i4 %b, i4 %a)
//        auto Identifier = Params->get(ParamCount - 1 - Idx++);
//        auto Name = Identifier->string();
//        Arg.setName(Name);
//    }

    return F;
}

llvm::Function *getRuntimeFunction(std::string Name) {
    
    // If not, check whether we can codegen the declaration from some existing
    // prototype.
    
    if (eval::RuntimeFunctionTypes.find(Name) != eval::RuntimeFunctionTypes.end()) {
        
        auto FT = eval::RuntimeFunctionTypes[Name];
        
        // First, see if the function has already been added to the current module.
        if (auto *F = eval::TheModule->getFunction(Name)) {
            return F;
        }
        
        auto F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, Name, eval::TheModule.get());
        
        return F;
        
    } else {
        llvm::errs() << Name << "\n";
        assert(false && "Unhandled Name");
    }
    
    // If no existing prototype exists, return null.
    return nullptr;
}

//const std::shared_ptr<UserFunction> DefinitionAST::define() const {
//    
//    auto name = Proto->getName()->getName();
//    
//    auto F = std::make_shared<UserFunction>(Proto, Body);
//    
//    eval::NamedValues[name] = F;
//    
//    return F;
//}

llvm::Function *DefinitionAST::codegen() {

//    auto FP = std::make_shared<FunctionDefinition>(ProtoAST, ProtoAST->getName()->string());
    
//    eval::FunctionDefinitions[ProtoAST->getName()->string()] = FP;
    
    llvm::Function *TheFunction = ProtoAST->codegen();
    if (!TheFunction) {
        return nullptr;
    }

    // Create a new basic block to start insertion into.
    llvm::BasicBlock *BB = llvm::BasicBlock::Create(eval::TheContext, "entry", TheFunction);
    eval::Builder.SetInsertPoint(BB);

    // Record the function arguments in the NamedValues map.
//    eval::NamedLLVMValues.clear();
//    for (auto &Arg : TheFunction->args()) {
//        eval::NamedLLVMValues[Arg.getName()] = &Arg;
//    }

    if (llvm::Value *RetVal = Body->codegen()) {
        // Finish off the function.
        eval::Builder.CreateRet(RetVal);

        // Validate the generated code, checking for consistency.
        if (!llvm::verifyFunction(*TheFunction, &llvm::outs())) {
            // verification is good

            // Optimize the function.
            eval::TheFPM->run(*TheFunction);

//            eval::NamedUserFunctions[Proto->getName()->string()] = std::make_shared<UserFunction>(Proto, Body);
    
            return TheFunction;
        } else {
            // message from verification error does not have newline
            llvm::outs() << "\n";

            // Error reading body, remove function.
            TheFunction->eraseFromParent();
            return nullptr;
        }
    }

    // Error reading body, remove function.
    TheFunction->eraseFromParent();
    return nullptr;
}

//llvm::Function *PrototypeAST::codegen() {
//    llvm_unreachable("PrototypeAST codegen");
//}

llvm::Value *ExpressionListExprAST::codegen() {
    
    auto Count = this->size();
    auto ValCountConstant = llvm::ConstantInt::get(eval::TheContext, llvm::APInt(64, Count));
    
    auto ArrTy = llvm::VectorType::get(eval::types::TuppenceValuePtrTy, Count);
    
    auto AllocaValue = eval::Builder.CreateAlloca(ArrTy);
    
    for (size_t i = 0; i < Count; i++) {
        auto Expr = Exprs[i];
        auto Val = Expr->codegen();
        if (!Val) {
            return nullptr;
        }
        
        //        // flatten out all child value lists
        //        if (auto ValList = llvm::dyn_cast<List>(Val.get())) {
        //            for (auto& V : *ValList) {
        //                Vals.push_back(V);
        //            }
        //        }
        //        else {
        //            Vals.push_back(Val);
        //        }
        
        auto ArrVal = eval::Builder.CreateLoad(ArrTy, AllocaValue);
        
        auto InsertVal = eval::Builder.CreateInsertElement(ArrVal, Val, i);
        
        eval::Builder.CreateStore(InsertVal, AllocaValue);
    }
    
    
    auto List_createFromVals = getRuntimeFunction("List_createFromVals");
    assert(List_createFromVals);
    
    
    
    auto Casted = eval::Builder.CreateBitCast(AllocaValue, eval::types::TuppenceValuePtrPtrTy);
    
    std::vector<llvm::Value *> ArgsV;
    ArgsV.push_back(Casted);
    ArgsV.push_back(ValCountConstant);
    
    auto CreateCall = eval::Builder.CreateCall(List_createFromVals, ArgsV, "calltmp");
    
    llvm::Value *ValueCall;
    {
        llvm::Function *Value_createFromList = getRuntimeFunction("Value_createFromList");
        assert(Value_createFromList);
        
        std::vector<llvm::Value *> ArgsV;
        ArgsV.push_back(CreateCall);
        
        ValueCall = eval::Builder.CreateCall(Value_createFromList, ArgsV, "calltmp");
    }
    
    return ValueCall;
}

llvm::Value *ExpressionSequenceExprAST::codegen() {

    auto Iter = Exprs.begin();
    for (; Iter != --Exprs.end(); Iter++) {
        auto Expr = *Iter;
        auto Val = Expr->codegen();
        if (!Val) {
            return nullptr;
        }
    }

    auto Last = *Iter;
    auto LastVal = Last->codegen();
    if (!LastVal) {
        return nullptr;
    }

    return LastVal;
}

llvm::Value *IntegerWordExprAST::codegen() {
    
    auto StrConst = AddGlobalString(Val, "integerString");
    
    llvm::Value *RationalWordCall;
    {
        std::vector<llvm::Value *> ArgsV;
        ArgsV.push_back(StrConst);
        
        llvm::Function *RationalWord_createFromDecimalString = getRuntimeFunction("RationalWord_createFromDecimalString");
        assert(RationalWord_createFromDecimalString);
        
        RationalWordCall = eval::Builder.CreateCall(RationalWord_createFromDecimalString, ArgsV, "rationalWord");
    }
    
    llvm::Value *ValueCall;
    {
        llvm::Function *Value_createFromRationalWord = getRuntimeFunction("Value_createFromRationalWord");
        assert(Value_createFromRationalWord);
        
        std::vector<llvm::Value *> ArgsV;
        ArgsV.push_back(RationalWordCall);
        
        ValueCall = eval::Builder.CreateCall(Value_createFromRationalWord, ArgsV, "value");
    }
    
    return ValueCall;
}

llvm::Constant* AddGlobalString(std::string S, std::string Name) {
    
    auto StrSize = S.size();
    
    auto StrTy = llvm::ArrayType::get(eval::types::Int8Ty, StrSize+1);
    
    llvm::GlobalVariable* gvar_array__str = new llvm::GlobalVariable(/*Module=*/*eval::TheModule,
                                                                     /*Type=*/StrTy,
                                                                     /*isConstant=*/true,
                                                                     /*Linkage=*/llvm::GlobalValue::PrivateLinkage,
                                                                     /*Initializer=*/0, // has initializer, specified below
                                                                     /*Name=*/Name);
    gvar_array__str->setAlignment(1);
    
    // Constant Definitions
    llvm::Constant *const_array_4 = llvm::ConstantDataArray::getString(eval::TheContext, S, true);
    std::vector<llvm::Constant*> const_ptr_5_indices;
    llvm::ConstantInt* const_int64_6 = llvm::ConstantInt::get(eval::TheContext, llvm::APInt(64, llvm::StringRef("0"), 10));
    const_ptr_5_indices.push_back(const_int64_6);
    const_ptr_5_indices.push_back(const_int64_6);
    llvm::Constant* const_ptr_5 = llvm::ConstantExpr::getGetElementPtr(StrTy, gvar_array__str, const_ptr_5_indices);
    
    // Global Variable Definitions
    gvar_array__str->setInitializer(const_array_4);
    
    return const_ptr_5;
}

llvm::Value *LogError(std::string ErrStr) {
    
    auto StrConst = AddGlobalString(ErrStr, "errorString");
    
    std::vector<llvm::Value *> ArgsForCall;
    ArgsForCall.push_back(StrConst);
    
    auto Value_createFromError = getRuntimeFunction("Value_createFromError");
    assert(Value_createFromError);
    
    return eval::Builder.CreateCall(Value_createFromError, ArgsForCall, "value");
}

llvm::Value *createFiniteWordValue(std::string Val) {

    auto StrConst = AddGlobalString(Val, "finiteWordString");
    
    llvm::Value *FiniteWordCall;
    if (Val.empty()) {
        
        llvm::Function *FiniteWord_createEmpty = getRuntimeFunction("FiniteWord_createEmpty");
        assert(FiniteWord_createEmpty);
        
        std::vector<llvm::Value *> ArgsV;
        
        FiniteWordCall = eval::Builder.CreateCall(FiniteWord_createEmpty, ArgsV, "finiteWord");
        
    } else {
        
        auto NumBits = llvm::ConstantInt::get(eval::TheContext, llvm::APInt(sizeof(size_t) * CHAR_BIT, Val.size()));
        
        std::vector<llvm::Value *> ArgsV;
        ArgsV.push_back(NumBits);
        ArgsV.push_back(StrConst);
        
        llvm::Function *FiniteWord_createFromBinaryString = getRuntimeFunction("FiniteWord_createFromBinaryString");
        assert(FiniteWord_createFromBinaryString);
        
        FiniteWordCall = eval::Builder.CreateCall(FiniteWord_createFromBinaryString, ArgsV, "finiteWord");
    }
    
    llvm::Value *ValueCall;
    {
        llvm::Function *Value_CreateFiniteWord = getRuntimeFunction("Value_createFromFiniteWord");
        assert(Value_CreateFiniteWord);
        
        std::vector<llvm::Value *> ArgsV;
        ArgsV.push_back(FiniteWordCall);
        
        ValueCall = eval::Builder.CreateCall(Value_CreateFiniteWord, ArgsV, "value");
    }
    
    return ValueCall;
}

llvm::Value *FiniteWordExprAST::codegen() {
    return createFiniteWordValue(Val);
}

llvm::Value *IdentifierExprAST::codegen() {
    
    auto GlobalPtrPtrPtr = eval::TheModule->getGlobalVariable(Name);
    
    if (!GlobalPtrPtrPtr) {
        
        auto NamedI = eval::NamedValues.find(Name);
        if (NamedI == eval::NamedValues.end()) {
            
            return LogError("Unknown identifier: " + Name);
        }
        
        GlobalPtrPtrPtr = createGlobal(Name, NamedI->second);
    }
    
    auto L = eval::Builder.CreateLoad(eval::types::TuppenceValuePtrPtrTy, GlobalPtrPtrPtr, "variable");
    
    return eval::Builder.CreateLoad(eval::types::TuppenceValuePtrTy, L, "value");
}

llvm::Value *UnaryExprAST::codegen() {
    
    auto O = Operand->codegen();
    if (!O) {
        return nullptr;
    }
    
    std::vector<llvm::Value *> ArgsForCall;
    
    auto OpConst = llvm::ConstantInt::get(eval::TheContext, llvm::APInt(sizeof(char) * CHAR_BIT, Operator, 2));
    ArgsForCall.push_back(OpConst);
    
    ArgsForCall.push_back(O);
    
    llvm::Function *TheFunction;
    TheFunction = getRuntimeFunction("Value_Unary");
    assert(TheFunction);
    
    auto Call = eval::Builder.CreateCall(TheFunction, ArgsForCall, "unaryCall");
    return Call;
    
}

llvm::Value *BinaryExprAST::codegen() {
    
    // Special case '=' because we don't want to emit the LHS as an expression.
    if (Op == '=') {
        if (auto IdentifierLHS = llvm::dyn_cast<IdentifierExprAST>(LHS.get())) {
            // eval the RHS.

//            auto Val = RHS->codegen();
//            if (!Val) {
//                return nullptr;
//            }
//
//            auto Name = IdentifierLHS->getName();
//
//            // List
//            if (auto ListVal = llvm::dyn_cast<List>(Val.get())) {
//                //
//                // List, take first element
//                //
//
//                eval::NamedValues[IdentifierLHS->getName()] = (*ListVal)[0];
//
//                return std::make_shared<FiniteWord>();
//            }
//            else {
//                // regular, non-List
//                auto GlobalPtrPtrPtr = eval::TheModule->getGlobalVariable(Name);
//                if (!GlobalPtrPtrPtr) {
//
//                    auto NamedI = eval::NamedValues.find(Name);
//                    if (NamedI == eval::NamedValues.end()) {
//
//                        eval::NamedValues[Name] = new TuppenceValue *;
//                    }
//
//                    auto Variable = eval::NamedValues[Name];
//
//                    GlobalPtrPtrPtr = createGlobal(Name, Variable);
//                }
//
//                auto L = eval::Builder.CreateLoad(eval::types::TuppenceValuePtrPtrTy, GlobalPtrPtrPtr, "variable");
//
//                eval::Builder.CreateStore(Val, L);
//
//                return Val;
//            }
            
        }
        else if (auto ListLHS = llvm::dyn_cast<ExpressionListExprAST>(LHS.get())) {
            
            //
            // LHS is lexical list
            //
            
            auto Val = RHS->codegen();
            if (!Val) {
                return nullptr;
            }

//            if (auto ListVal = llvm::dyn_cast<List>(Val.get())) {

//                auto Exprs = ListLHS->getExprs();
//
//                if (Exprs.size() <= ListVal->size()) {
//
//                    for (size_t i = 0; i < Exprs.size(); i++) {
//                        auto Var = Exprs[i];
//                        if (auto V = llvm::dyn_cast<IdentifierExprAST>(Var.get())) {
//
//                            // prevent reassigning to builtins
//                            auto N = eval::NamedValues[V->getName()];
//                            if (N) {
//                                if (auto BuiltinFunctionN = llvm::dyn_cast<BuiltinFunction>(N.get())) {
//                                    return LogError("Cannot reassign builtin function: " + BuiltinFunctionN->string());
//                                } else if (auto BuiltinSymbolN = llvm::dyn_cast<BuiltinSymbol>(N.get())) {
//                                    return LogError("Cannot reassign builtin symbol: " + BuiltinSymbolN->string());
//                                }
//                            }
//
//                            eval::NamedValues[V->getName()] = (*ListVal)[i];
//                        }
//                        else {
//                            return LogError("Expected identifer: " + Var->string());
//                        }
//                    }
//
//                    return std::make_shared<FiniteWord>();
//                }
//                else {
//                    return LogError("Too many identifiers for assignment");
//                }
//            }
//            else {
//                return LogError("List expected on RHS: " + RHS->string());
//            }
        }
        else {
            return LogError("Invalid LHS for " + stringFromToken(Op) + ": " + LHS->string());
        }
    }
    
    auto L = LHS->codegen();
    if (!L) {
        return nullptr;
    }
    // if L is a list, then concatenate all together
//    if (auto ListL = llvm::dyn_cast<List>(L.get())) {
//        L = ListL->concatenate();
//        if (!L) {
//            return nullptr;
//        }
//    }
    
    // handle method calls first
    if (Op == '.') {
        
        if (auto CallRHS = llvm::dyn_cast<CallExprAST>(RHS.get())) {
            
            //
            // Create a new CallExprAST with L as the first argument
            //
            // a.foo(1) is equivalent to foo(a, 1)
            //
            
            auto Callee = CallRHS->getCallee();
            
            auto CallRHSExpressionList = CallRHS->getArgs();
            auto CallRHSExpressionListExprs = CallRHSExpressionList->getExprs();
            std::vector<std::shared_ptr<ExprAST>> NewExprs;
            
            NewExprs.push_back(LHS);
            NewExprs.insert(NewExprs.end(), CallRHSExpressionListExprs.begin(), CallRHSExpressionListExprs.end());
            auto NewExpressionList = std::make_shared<ExpressionListExprAST>(NewExprs);
            
            auto Call = std::make_shared<CallExprAST>(Callee, NewExpressionList);
            return Call->codegen();
            
        } else {
            return LogError("RHS must be a call: " + RHS->string());
        }
    }
    
    auto R = RHS->codegen();
    if (!R) {
        return nullptr;
    }
    
    // For arithmetic and structural operations, concatenate all lists together
//    if (auto ListR = llvm::dyn_cast<List>(R.get())) {
//        R = ListR->concatenate();
//        if (!R) {
//            return nullptr;
//        }
//    }
    
    
    
    
    
    
    std::vector<llvm::Value *> ArgsForCall;
    
    auto OpConst = llvm::ConstantInt::get(eval::TheContext, llvm::APInt(sizeof(char) * CHAR_BIT, Op, 2));
    ArgsForCall.push_back(OpConst);
    
    ArgsForCall.push_back(L);
    ArgsForCall.push_back(R);
    
    llvm::Function *TheFunction;
    TheFunction = getRuntimeFunction("Value_Binary");
    assert(TheFunction);
    
    auto Call = eval::Builder.CreateCall(TheFunction, ArgsForCall, "binaryCall");
    return Call;
}

llvm::Value *InfixExprAST::codegen() {

    std::vector<llvm::Value *> ArgsForCall;
    
    auto OpConst = llvm::ConstantInt::get(eval::TheContext, llvm::APInt(sizeof(char) * CHAR_BIT, Op, 2));
    ArgsForCall.push_back(OpConst);
    
    auto ArrTy = llvm::VectorType::get(eval::types::TuppenceValuePtrTy, Args.size());
    auto AllocaValue = eval::Builder.CreateAlloca(ArrTy);
    
    for (size_t  i = 0; i < Args.size(); i++) {
        auto Arg = Args[i];
        auto Val = Arg->codegen();
        if (!Val) {
            return nullptr;
        }
        
        auto ArrVal = eval::Builder.CreateLoad(ArrTy, AllocaValue);
        
        auto InsertVal = eval::Builder.CreateInsertElement(ArrVal, Val, i);
        
        eval::Builder.CreateStore(InsertVal, AllocaValue);
    }
    
    auto Casted = eval::Builder.CreateBitCast(AllocaValue, eval::types::TuppenceValuePtrPtrTy);
    ArgsForCall.push_back(Casted);
    
    auto SizeConst = llvm::ConstantInt::get(eval::TheContext, llvm::APInt(sizeof(size_t) * CHAR_BIT, Args.size()));
    ArgsForCall.push_back(SizeConst);
    
    llvm::Function *TheFunction;
    TheFunction = getRuntimeFunction("Value_Infix");
    assert(TheFunction);
    
    auto Call = eval::Builder.CreateCall(TheFunction, ArgsForCall);
    return Call;
}

llvm::Value *CallExprAST::codegen() {

    auto Args = getArgs()->getExprs();
    
    std::vector<llvm::Value *> ArgsForCall;

    if (auto IdentifierCallee = llvm::dyn_cast<IdentifierExprAST>(Callee.get())) {

        auto Name = IdentifierCallee->getName();
        
        auto GlobalPtrPtrPtr = eval::TheModule->getGlobalVariable(Name);
        if (!GlobalPtrPtrPtr) {
            
            auto NamedI = eval::NamedValues.find(Name);
            if (NamedI == eval::NamedValues.end()) {
                
                return LogError("Not a value: " + IdentifierCallee->getName());
            }
            
            GlobalPtrPtrPtr = createGlobal(Name, NamedI->second);
        }
        
        auto L = eval::Builder.CreateLoad(eval::types::TuppenceValuePtrPtrTy, GlobalPtrPtrPtr);
        
        auto Callee = eval::Builder.CreateLoad(eval::types::TuppenceValuePtrTy, L);
        
        ArgsForCall.push_back(Callee);
        
        
        auto ArrTy = llvm::ArrayType::get(eval::types::TuppenceValuePtrTy, Args.size());
        auto AllocaValue = eval::Builder.CreateAlloca(ArrTy);
        
        llvm::Value *InsertVal = eval::Builder.CreateLoad(ArrTy, AllocaValue);
        
        for (size_t  i = 0; i < Args.size(); i++) {
            auto Arg = Args[i];
            auto Val = Arg->codegen();
            if (!Val) {
                return nullptr;
            }
            
            InsertVal = eval::Builder.CreateInsertValue(InsertVal, Val, { (unsigned)i });
        }
        
        eval::Builder.CreateStore(InsertVal, AllocaValue);
        
        auto Casted = eval::Builder.CreateBitCast(AllocaValue, eval::types::TuppenceValuePtrPtrTy);
        ArgsForCall.push_back(Casted);
        
        auto SizeConst = llvm::ConstantInt::get(eval::TheContext, llvm::APInt(sizeof(size_t) * CHAR_BIT, Args.size()));
        ArgsForCall.push_back(SizeConst);
        
        
        
        auto Dispatch = getRuntimeFunction("Runtime_dispatch");
        
        return eval::Builder.CreateCall(Dispatch, ArgsForCall, "dispatch");
        
    }

    return LogError("Callee is not an identifier: " + Callee->string());
}

//const std::shared_ptr<Value> IfExprAST::eval() const {
//    auto CondV = Cond->eval();
//    if (!CondV) {
//        return nullptr;
//    }
//
//    // if Cond is a list, then concatenate all values
//    if (auto ListCond = llvm::dyn_cast<List>(CondV.get())) {
//        CondV = ListCond->concatenate();
//        if (!CondV) {
//            return nullptr;
//        }
//    }
//
//    auto Test = llvm::dyn_cast<FiniteWord>(CondV.get());
//    if (!Test) {
//        return LogError("Test is not a FiniteWord: " + Test->string());
//    }
//
//    if (Test->size() != 1) {
//        return LogError("Test is not size 1: " + Test->string());
//    }
//
//    if (*Test == eval::ONE_1BIT) {
//        return Then->eval();
//    }
//    else {
//        return Else->eval();
//    }
//}

llvm::Value *IfExprAST::codegen() {
    
    //
    // Take the foo out of:
    // if (foo)
    //
    std::shared_ptr<ExprAST> CondFirst;
    if (auto ListCond = llvm::dyn_cast<ExpressionListExprAST>(Cond.get())) {
        
        if (ListCond->size() == 1) {
            
            CondFirst = ListCond->get(0);
            
        } else {
            return LogError("Expected List with size 1");
        }
        
    } else {
        return LogError("Expected List");
    }
    
    auto C = CondFirst->codegen();
    if (!C) {
        return nullptr;
    }
    
    std::vector<llvm::Value*> TagArgs = {
        llvm::ConstantInt::get(eval::TheContext, llvm::APInt(32, 0)),
        llvm::ConstantInt::get(eval::TheContext, llvm::APInt(32, 0)),
    };
    
    auto TagPtr = eval::Builder.CreateGEP(C, TagArgs);
    
    auto Tag = eval::Builder.CreateLoad(eval::types::Int8Ty, TagPtr);
    
    auto Cmp = eval::Builder.CreateICmp(llvm::CmpInst::Predicate::ICMP_EQ, Tag, llvm::ConstantInt::get(eval::TheContext, llvm::APInt(8, FiniteWordTag)));
    
    llvm::Function *TheFunction = eval::Builder.GetInsertBlock()->getParent();
    
    // Create blocks for the then and else cases.  Insert the 'then' block at the
    // end of the function.
    llvm::BasicBlock *TypeGoodBB = llvm::BasicBlock::Create(eval::TheContext, "type_good");
    llvm::BasicBlock *TypeBadBB = llvm::BasicBlock::Create(eval::TheContext, "type_bad");
    llvm::BasicBlock *SizeGoodBB = llvm::BasicBlock::Create(eval::TheContext, "size_good");
    llvm::BasicBlock *SizeBadBB = llvm::BasicBlock::Create(eval::TheContext, "size_bad");
    llvm::BasicBlock *CondTrueBB = llvm::BasicBlock::Create(eval::TheContext, "cond_true");
    llvm::BasicBlock *CondFalseBB = llvm::BasicBlock::Create(eval::TheContext, "cond_false");
    llvm::BasicBlock *MergeBB = llvm::BasicBlock::Create(eval::TheContext, "merge");
    
    eval::Builder.CreateCondBr(Cmp, TypeGoodBB, TypeBadBB);
    
    
    
    
    
    // Emit TypeBad block.
    TheFunction->getBasicBlockList().push_back(TypeBadBB);
    eval::Builder.SetInsertPoint(TypeBadBB);
    
    llvm::Value *TypeBadV = LogError("Expected a Finite Word");
    
    eval::Builder.CreateBr(MergeBB);
    
    // Codegen can change the current block, update
    TypeBadBB = eval::Builder.GetInsertBlock();
    
    
    
    
    
    // Emit TypeGood block
    //
    // Check that size is 1
    //
    TheFunction->getBasicBlockList().push_back(TypeGoodBB);
    eval::Builder.SetInsertPoint(TypeGoodBB);
    
    std::vector<llvm::Value*> PayloadArgs = {
        llvm::ConstantInt::get(eval::TheContext, llvm::APInt(32, 0)),
        llvm::ConstantInt::get(eval::TheContext, llvm::APInt(32, 1)),
    };
    
    auto PayloadPtr = eval::Builder.CreateGEP(C, PayloadArgs);
    
    auto Payload = eval::Builder.CreateLoad(eval::types::VoidPtrTy, PayloadPtr);
    auto Casted = eval::Builder.CreateBitCast(Payload, eval::types::FiniteWordPtrTy);
    
    std::vector<llvm::Value *> ArgsV;
    ArgsV.push_back(Casted);
    
    llvm::Function *FiniteWord_size = getRuntimeFunction("FiniteWord_size");
    assert(FiniteWord_size);
    
    auto SizeCall = eval::Builder.CreateCall(FiniteWord_size, ArgsV, "size");
    
    auto SizeCmp = eval::Builder.CreateICmp(llvm::CmpInst::Predicate::ICMP_EQ, SizeCall, llvm::ConstantInt::get(eval::TheContext, llvm::APInt(sizeof(size_t) * CHAR_BIT, 1)));
    
    eval::Builder.CreateCondBr(SizeCmp, SizeGoodBB, SizeBadBB);
    
    // Codegen can change the current block, update
    TypeGoodBB = eval::Builder.GetInsertBlock();
    
    
    
    
    
    // Emit SizeBad block.
    TheFunction->getBasicBlockList().push_back(SizeBadBB);
    eval::Builder.SetInsertPoint(SizeBadBB);
    
    llvm::Value *SizeBadV = LogError("Expected a Finite Word with size 1");
    
    eval::Builder.CreateBr(MergeBB);
    
    // Codegen can change the current block, update
    SizeBadBB = eval::Builder.GetInsertBlock();
    
    
    
    
    
    
    
    
    
    
    
    // Emit SizeGood block
    //
    // Check Cond
    //
    TheFunction->getBasicBlockList().push_back(SizeGoodBB);
    eval::Builder.SetInsertPoint(SizeGoodBB);
    
    std::vector<llvm::Value *> GetBitArgsV;
    GetBitArgsV.push_back(Casted);
    GetBitArgsV.push_back(llvm::ConstantInt::get(eval::TheContext, llvm::APInt(sizeof(size_t) * CHAR_BIT, 0)));
    
    llvm::Function *FiniteWord_getBit = getRuntimeFunction("FiniteWord_getBit");
    assert(FiniteWord_getBit);
    
    auto GetBitCall = eval::Builder.CreateCall(FiniteWord_getBit, GetBitArgsV, "bit");
    
    auto BitCmp = eval::Builder.CreateICmp(llvm::CmpInst::Predicate::ICMP_EQ, GetBitCall, llvm::ConstantInt::get(eval::TheContext, llvm::APInt(8, 1)));
    
    eval::Builder.CreateCondBr(BitCmp, CondTrueBB, CondFalseBB);
    
    // Codegen can change the current block, update
    SizeGoodBB = eval::Builder.GetInsertBlock();
    
    
    
    
    
    
    
    // Emit CondTrue block
    TheFunction->getBasicBlockList().push_back(CondTrueBB);
    eval::Builder.SetInsertPoint(CondTrueBB);
    
    llvm::Value *CondTrueV = Then->codegen();
    if (!CondTrueV) {
        return nullptr;
    }
    
    eval::Builder.CreateBr(MergeBB);
    // Codegen can change the current block, update
    CondTrueBB = eval::Builder.GetInsertBlock();
    
    
    
    
    
    // Emit CondFalse block.
    TheFunction->getBasicBlockList().push_back(CondFalseBB);
    eval::Builder.SetInsertPoint(CondFalseBB);
    
    llvm::Value *CondFalseV = Else->codegen();
    if (!CondFalseV) {
        return nullptr;
    }
    
    eval::Builder.CreateBr(MergeBB);
    // Codegen can change the current block, update
    CondFalseBB = eval::Builder.GetInsertBlock();
    
    
    
    
    
    
    
    
    
    
    // Emit merge block.
    TheFunction->getBasicBlockList().push_back(MergeBB);
    eval::Builder.SetInsertPoint(MergeBB);
    llvm::PHINode *PN = eval::Builder.CreatePHI(eval::types::TuppenceValuePtrTy, 4, "iftmp");
    
    PN->addIncoming(TypeBadV, TypeBadBB);
    PN->addIncoming(SizeBadV, SizeBadBB);
    PN->addIncoming(CondTrueV, CondTrueBB);
    PN->addIncoming(CondFalseV, CondFalseBB);
    
    return PN;
}

//const std::shared_ptr<Value> ForExprAST::eval() const {
//
//    auto StartV = Start->eval();
//    if (!StartV) {
//        return nullptr;
//    }
//
//    std::shared_ptr<Value> Res = std::make_shared<FiniteWord>();
//
//    while (1) {
//        auto EV = End->eval();
//        if (!EV) {
//            return nullptr;
//        }
//
//        auto EndV = llvm::dyn_cast<FiniteWord>(EV.get());
//        if (!EndV) {
//            return LogError("End is not a FiniteWord: " + EndV->string());
//        }
//
//        if (EndV->size() != 1) {
//            return LogError("End is not size 1: " + EndV->string());
//        }
//
//        if (*EndV == eval::ZERO_1BIT) {
//            break;
//        }
//
//        if (!(Res = Body->eval())) {
//            return nullptr;
//        }
//
//        if (!(Step->eval())) {
//            return nullptr;
//        }
//    }
//
//    return std::make_shared<FiniteWord>();
//}

//llvm::Value *ForExprAST::codegen() {
//    llvm_unreachable("ForExprAST codegen");
//}

//const std::shared_ptr<Value> WhileExprAST::eval() const {
//    while (1) {
//
//        auto TestV = Test->eval();
//        if (!TestV) {
//            return nullptr;
//        }
//
//        if (auto FiniteWordTest = llvm::dyn_cast<FiniteWord>(TestV.get())) {
//            if (FiniteWordTest->size() == 1) {
//                if (*FiniteWordTest == eval::ZERO_1BIT) {
//                    break;
//                }
//            }
//            else {
//                return LogError("Test is not size 1: " + FiniteWordTest->string());
//            }
//        }
//        else {
//            return LogError("Test is not a FiniteWord: " + TestV->string());
//        }
//
//        auto Res = Body->eval();
//        if (!Res) {
//            return nullptr;
//        }
//    }
//
//    return std::make_shared<FiniteWord>();
//}

llvm::Value *WhileExprAST::codegen() {
    llvm_unreachable("WhileExprAST codegen");
}

//const std::shared_ptr<Value> FunctionExprAST::eval() const {
//    
//    auto F = std::make_shared<UserFunction>(Params, Body);
//    
//    return F;
//}

//class ScopePusher {
//    const UserFunction *userFunction;
//    const std::vector<std::shared_ptr<Value>> Args;
//    std::vector<std::shared_ptr<Value>> OldBindings;
//public:
//    ScopePusher(const UserFunction* userFunction, std::vector<std::shared_ptr<Value>> Args) : userFunction(userFunction), Args(Args) {
//        // Push all bindings
//        auto Params = userFunction->getProto()->getParams();
//        if (!Params->empty()) {
//            auto IterArgs = Args.begin();
//            auto IterParam = Params->begin();
//            for (; IterParam != --Params->end(); IterParam++) {
//                auto Param = *IterParam;
//                if (auto P = llvm::dyn_cast<IdentifierExprAST>(Param.get())) {
//                    auto VarName = P->getName();
//                    OldBindings.push_back(eval::NamedValues[VarName]);
//                    auto Arg = *IterArgs;
//                    eval::NamedValues[VarName] = Arg;
//                    IterArgs++;
//                }
//            }
//            auto Last = *IterParam;
//            if (auto P = llvm::dyn_cast<IdentifierExprAST>(Last.get())) {
//                auto VarName = P->getName();
//                OldBindings.push_back(eval::NamedValues[VarName]);
//                auto LastArg = *IterArgs;
//                eval::NamedValues[VarName] = LastArg;
//                IterArgs++;
//            }
//            else if (auto P = llvm::dyn_cast<UnaryExprAST>(Last.get())) {
//                assert(P->getOperator() == tok_dot_dot_dot);
//                if (auto VarIdentifier = llvm::dyn_cast<IdentifierExprAST>(P->getOperand().get())) {
//                    auto VarName = VarIdentifier->getName();
//                    OldBindings.push_back(eval::NamedValues[VarName]);
//                    std::vector<std::shared_ptr<Value>> Vals;
//                    while (IterArgs != Args.end()) {
//                        auto Val = *IterArgs;
//                        Vals.push_back(Val);
//                        IterArgs++;
//                    }
//                    eval::NamedValues[VarName] = std::make_shared<List>(Vals);
//                }
//            }
//        }
//    }
//    
//    ~ScopePusher() {
//        // Pop bindings back
//        auto Params = userFunction->getProto()->getParams();
//        for (size_t i = 0, e = Params->size(); i != e; ++i) {
//            auto Var = Params->get(i);
//            if (auto VarIdentifier = llvm::dyn_cast<IdentifierExprAST>(Var.get())) {
//                auto VarName = VarIdentifier->getName();
//                eval::NamedValues[VarName] = OldBindings[i];
//            } else if (auto VarDotDotDot = llvm::dyn_cast<UnaryExprAST>(Var.get())) {
//                if (auto VarIdentifier = llvm::dyn_cast<IdentifierExprAST>(VarDotDotDot->getOperand().get())) {
//                    auto VarName = VarIdentifier->getName();
//                    eval::NamedValues[VarName] = OldBindings[i];
//                }
//            }
//        }
//    }
//};

//const std::shared_ptr<Value> UserFunction::call(std::vector<std::shared_ptr<Value>> Args) const {
//    ScopePusher SomeLongNameThatIsNotLikelyToBeUsedInTheScopePusher(this, Args);
//    
//    auto Res = Body->eval();
//    if (!Res) {
//        return nullptr;
//    }
//    return Res;
//}

//const std::shared_ptr<Value> BuiltinFunction::call(std::vector<std::shared_ptr<Value>> Args) const {
//
//    auto Res = (*FunctionPointer)(Args);
//    if (!Res) {
//        return nullptr;
//    }
//
//    return Res;
//}






llvm::GlobalVariable *createGlobal(std::string Name, TuppenceValue **V) {
    auto G = new llvm::GlobalVariable(
                                      *eval::TheModule,
                                      eval::types::TuppenceValuePtrPtrTy,
                                      false,
                                      llvm::GlobalValue::ExternalLinkage,
                                      /*Initializer=*/0, // has initializer, specified below
                                      Name.c_str());
    // Constant Definitions
    auto C = llvm::ConstantInt::get(eval::types::Int64Ty, (int64_t)V);
    auto CPtr = llvm::ConstantExpr::getIntToPtr(C, eval::types::TuppenceValuePtrPtrTy);
    // Global Variable Definitions
    G->setInitializer(CPtr);
    
    return G;
}





