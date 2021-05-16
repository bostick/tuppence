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
#include "../common/BuiltinSymbol.h"
#include "../common/Lexer.h"

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



//
// Initialize the types that will be used throughout the program
//
void eval::InitializeTypes() {
    
    eval::types::Int1Ty = llvm::IntegerType::get(eval::TheContext, 1);
    eval::types::Int8Ty = llvm::IntegerType::get(eval::TheContext, 8);
    eval::types::Int32Ty = llvm::IntegerType::get(eval::TheContext, 32);
    eval::types::Int64Ty = llvm::IntegerType::get(eval::TheContext, 64);
    
    eval::types::CharTy = llvm::IntegerType::get(eval::TheContext, sizeof(char) * CHAR_BIT);
    eval::types::SizeTTy = llvm::IntegerType::get(eval::TheContext, sizeof(size_t) * CHAR_BIT);
    
    llvm::StructType *finiteWordTy = llvm::StructType::create(eval::TheContext, "FiniteWord");
    eval::types::FiniteWordPtrTy = llvm::PointerType::get(finiteWordTy, 0);
    
    llvm::StructType *rationalWordTy = llvm::StructType::create(eval::TheContext, "RationalWord");
    eval::types::RationalWordPtrTy = llvm::PointerType::get(rationalWordTy, 0);
    
    llvm::StructType *listTy = llvm::StructType::create(eval::TheContext, "List");
    eval::types::ListPtrTy = llvm::PointerType::get(listTy, 0);
    
    llvm::StructType *builtinSymbolTy = llvm::StructType::create(eval::TheContext, "BuiltinSymbol");
    eval::types::BuiltinSymbolPtrTy = llvm::PointerType::get(builtinSymbolTy, 0);
    
    llvm::StructType *builtinFunctionTy = llvm::StructType::create(eval::TheContext, "BuiltinFunction");
    eval::types::BuiltinFunctionPtrTy = llvm::PointerType::get(builtinFunctionTy, 0);
    
    eval::types::VoidPtrTy = llvm::PointerType::get(eval::types::Int8Ty, 0);
    eval::types::CharPtrTy = llvm::PointerType::get(eval::types::CharTy, 0);
    eval::types::Int8PtrTy = llvm::PointerType::get(eval::types::Int8Ty, 0);
    
    eval::types::TuppenceValueTy = llvm::StructType::create(eval::TheContext, "TuppenceValue");
    std::vector<llvm::Type*> members;
    members.push_back( eval::types::Int8Ty ); // tag
    members.push_back( eval::types::VoidPtrTy ); // payload
    eval::types::TuppenceValueTy->setBody(members);
    
    eval::types::TuppenceValuePtrTy = llvm::PointerType::get(eval::types::TuppenceValueTy, 0);
    eval::types::TuppenceValuePtrPtrTy = llvm::PointerType::get(eval::types::TuppenceValuePtrTy, 0);

}

//
// Setup values for RuntimeFunctionTypes
//
void eval::InitializeRuntimeFunctions() {
    {
        // TuppenceValue *Value_Infix(char Op, TuppenceValue **arg0, size_t Count);
        
        std::vector<llvm::Type *> ParamTys;
        ParamTys.push_back(eval::types::CharTy);
        ParamTys.push_back(eval::types::TuppenceValuePtrPtrTy);
        ParamTys.push_back(eval::types::SizeTTy);
        auto RetTy = eval::types::TuppenceValuePtrTy;
        llvm::FunctionType *FT = llvm::FunctionType::get(RetTy, ParamTys, false);
        
        RuntimeFunctionTypes["Value_Infix"] = FT;
    }
    
    {
        // FiniteWordImpl *FiniteWord_createFromBinaryString(size_t numBits, char *str);
        
        std::vector<llvm::Type *> ParamTys;
        ParamTys.push_back(eval::types::SizeTTy);
        ParamTys.push_back(eval::types::CharPtrTy);
        auto RetTy = eval::types::FiniteWordPtrTy;
        llvm::FunctionType *FT = llvm::FunctionType::get(RetTy, ParamTys, false);
        
        RuntimeFunctionTypes["FiniteWord_createFromBinaryString"] = FT;
    }
    
    {
        // FiniteWordImpl *FiniteWord_createEmpty();
        
        std::vector<llvm::Type *> ParamTys;
        auto RetTy = eval::types::FiniteWordPtrTy;
        llvm::FunctionType *FT = llvm::FunctionType::get(RetTy, ParamTys, false);
        
        RuntimeFunctionTypes["FiniteWord_createEmpty"] = FT;
    }
    
    {
        // size_t FiniteWord_size(FiniteWord *word);
        
        std::vector<llvm::Type *> ParamTys;
        ParamTys.push_back(eval::types::FiniteWordPtrTy);
        auto RetTy = eval::types::SizeTTy;
        llvm::FunctionType *FT = llvm::FunctionType::get(RetTy, ParamTys, false);
        
        RuntimeFunctionTypes["FiniteWord_size"] = FT;
    }
    
    {
        // uint8_t FiniteWord_getBit(FiniteWord *word, size_t bitPosition);
        
        std::vector<llvm::Type *> ParamTys;
        ParamTys.push_back(eval::types::FiniteWordPtrTy);
        ParamTys.push_back(eval::types::SizeTTy);
        auto RetTy = eval::types::Int8Ty;
        llvm::FunctionType *FT = llvm::FunctionType::get(RetTy, ParamTys, false);
        
        RuntimeFunctionTypes["FiniteWord_getBit"] = FT;
    }
    
    {
        // TuppenceValue *Value_CreateFiniteWord(FiniteWordImpl *fin);
        
        std::vector<llvm::Type *> ParamTys;
        ParamTys.push_back(eval::types::FiniteWordPtrTy);
        auto RetTy = eval::types::TuppenceValuePtrTy;
        llvm::FunctionType *FT = llvm::FunctionType::get(RetTy, ParamTys, false);
        
        RuntimeFunctionTypes["Value_createFromFiniteWord"] = FT;
    }
    
    {
        // List *List_createFromVals(TuppenceValue **Vals, size_t Count);
        
        std::vector<llvm::Type *> ParamTys;
        ParamTys.push_back(eval::types::TuppenceValuePtrPtrTy);
        ParamTys.push_back(eval::types::SizeTTy);
        auto RetTy = eval::types::ListPtrTy;
        llvm::FunctionType *FT = llvm::FunctionType::get(RetTy, ParamTys, false);
        
        RuntimeFunctionTypes["List_createFromVals"] = FT;
    }
    
    {
        // TuppenceValue *Value_createFromList(List *list);
        
        std::vector<llvm::Type *> ParamTys;
        ParamTys.push_back(eval::types::ListPtrTy);
        auto RetTy = eval::types::TuppenceValuePtrTy;
        llvm::FunctionType *FT = llvm::FunctionType::get(RetTy, ParamTys, false);
        
        RuntimeFunctionTypes["Value_createFromList"] = FT;
    }
    
    {
        // uint8_t Value_tag(TuppenceValue *);
        
        std::vector<llvm::Type *> ParamTys;
        ParamTys.push_back(eval::types::TuppenceValuePtrTy);
        auto RetTy = eval::types::Int8Ty;
        llvm::FunctionType *FT = llvm::FunctionType::get(RetTy, ParamTys, false);
        
        RuntimeFunctionTypes["Value_tag"] = FT;
    }
    
    {
        // void Value_dump(TuppenceValue *);
        
        std::vector<llvm::Type *> ParamTys;
        ParamTys.push_back(eval::types::TuppenceValuePtrTy);
        auto RetTy = eval::types::Int8Ty;
        llvm::FunctionType *FT = llvm::FunctionType::get(RetTy, ParamTys, false);
        
        RuntimeFunctionTypes["Value_dump"] = FT;
    }
    
    {
        // RationalWord *RationalWord_createFromDecimalString(char *StrVal);
        
        std::vector<llvm::Type *> ParamTys;
        ParamTys.push_back(eval::types::CharPtrTy);
        auto RetTy = eval::types::RationalWordPtrTy;
        llvm::FunctionType *FT = llvm::FunctionType::get(RetTy, ParamTys, false);
        
        RuntimeFunctionTypes["RationalWord_createFromDecimalString"] = FT;
    }
    
    {
        // TuppenceValue *Value_createFromRationalWord(RationalWord *rat);
        
        std::vector<llvm::Type *> ParamTys;
        ParamTys.push_back(eval::types::RationalWordPtrTy);
        auto RetTy = eval::types::TuppenceValuePtrTy;
        llvm::FunctionType *FT = llvm::FunctionType::get(RetTy, ParamTys, false);
        
        RuntimeFunctionTypes["Value_createFromRationalWord"] = FT;
    }
    
    {
        // TuppenceValue *Value_Binary(char Op, TuppenceValue *Arg1, TuppenceValue *Arg2);
        
        std::vector<llvm::Type *> ParamTys;
        ParamTys.push_back(eval::types::CharTy);
        ParamTys.push_back(eval::types::TuppenceValuePtrTy);
        ParamTys.push_back(eval::types::TuppenceValuePtrTy);
        auto RetTy = eval::types::TuppenceValuePtrTy;
        llvm::FunctionType *FT = llvm::FunctionType::get(RetTy, ParamTys, false);
        
        RuntimeFunctionTypes["Value_Binary"] = FT;
    }
    
    {
        // TuppenceValue *Value_Unary(char Op, TuppenceValue *Arg1);
        
        std::vector<llvm::Type *> ParamTys;
        ParamTys.push_back(eval::types::CharTy);
        ParamTys.push_back(eval::types::TuppenceValuePtrTy);
        auto RetTy = eval::types::TuppenceValuePtrTy;
        llvm::FunctionType *FT = llvm::FunctionType::get(RetTy, ParamTys, false);
        
        RuntimeFunctionTypes["Value_Unary"] = FT;
    }
    
    {
        // TuppenceValue *Value_createFromError(const char *);
        
        std::vector<llvm::Type *> ParamTys;
        ParamTys.push_back(eval::types::CharPtrTy);
        auto RetTy = eval::types::TuppenceValuePtrTy;
        llvm::FunctionType *FT = llvm::FunctionType::get(RetTy, ParamTys, false);
        
        RuntimeFunctionTypes["Value_createFromError"] = FT;
    }
    
    {
        //        BuiltinSymbol *BuiltinSymbol_createFromString(const char*);
        
        std::vector<llvm::Type *> ParamTys;
        ParamTys.push_back(eval::types::CharPtrTy);
        auto RetTy = eval::types::BuiltinSymbolPtrTy;
        llvm::FunctionType *FT = llvm::FunctionType::get(RetTy, ParamTys, false);
        
        RuntimeFunctionTypes["BuiltinSymbol_createFromString"] = FT;
    }
    
    {
        //        TuppenceValue *Value_createFromBuiltinSymbol(BuiltinSymbol *);
        
        std::vector<llvm::Type *> ParamTys;
        ParamTys.push_back(eval::types::BuiltinSymbolPtrTy);
        auto RetTy = eval::types::TuppenceValuePtrTy;
        llvm::FunctionType *FT = llvm::FunctionType::get(RetTy, ParamTys, false);
        
        RuntimeFunctionTypes["Value_createFromBuiltinSymbol"] = FT;
    }
    
    {
        //        BuiltinFunction *BuiltinFunction_createFromName(const char*);
        
        std::vector<llvm::Type *> ParamTys;
        ParamTys.push_back(eval::types::CharPtrTy);
        auto RetTy = eval::types::BuiltinFunctionPtrTy;
        llvm::FunctionType *FT = llvm::FunctionType::get(RetTy, ParamTys, false);
        
        RuntimeFunctionTypes["BuiltinFunction_createFromString"] = FT;
    }
    
    {
        //        TuppenceValue *Value_createFromBuiltinFunction(BuiltinFunction *);
        
        std::vector<llvm::Type *> ParamTys;
        ParamTys.push_back(eval::types::BuiltinFunctionPtrTy);
        auto RetTy = eval::types::TuppenceValuePtrTy;
        llvm::FunctionType *FT = llvm::FunctionType::get(RetTy, ParamTys, false);
        
        RuntimeFunctionTypes["Value_createFromBuiltinFunction"] = FT;
    }
    
    {
        //        TuppenceValue *Runtime_dispatch(TuppenceValue *Callee, TuppenceValue **Args, size_t Count);
        
        std::vector<llvm::Type *> ParamTys;
        ParamTys.push_back(eval::types::TuppenceValuePtrTy);
        ParamTys.push_back(eval::types::TuppenceValuePtrPtrTy);
        ParamTys.push_back(eval::types::SizeTTy);
        auto RetTy = eval::types::TuppenceValuePtrTy;
        llvm::FunctionType *FT = llvm::FunctionType::get(RetTy, ParamTys, false);
        
        RuntimeFunctionTypes["Runtime_dispatch"] = FT;
    }
}

//
// This is called once at startup.
//
void eval::InitializeNamedValues() {
    
    eval::NamedValues["infinity"] = &TuppenceValue_INFINITY;
    
    eval::NamedValues["print"] = &TuppenceValue_PRINT;
    
    eval::NamedValues["exit"] = &TuppenceValue_EXIT;
    
    eval::NamedValues["rationalize"] = &TuppenceValue_RATIONALIZE;
    
    eval::NamedValues["size"] = &TuppenceValue_SIZE;
    
    eval::NamedValues["period"] = &TuppenceValue_PERIOD;
    
    eval::NamedValues["transient"] = &TuppenceValue_TRANSIENT;
    
    eval::NamedValues["numerator"] = &TuppenceValue_NUMERATOR;
    
    eval::NamedValues["denominator"] = &TuppenceValue_DENOMINATOR;
}

