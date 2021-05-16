//===------ Eval.h --------------------------------------------------------===//
//
// The Tuppence Programming Language
//
// This file is distributed under the MIT Open Source License.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

#pragma once

#include "../common/TuppenceValue.h"

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "KaleidoscopeJIT.h"

#include <map>
#include <vector>
#include <unordered_set>

namespace tuppence {

    // class FunctionDefinition {
    //     const std::shared_ptr<PrototypeAST> ProtoAST;
    //     const std::string ExternalName;
    // public:
    //     FunctionDefinition(const std::shared_ptr<PrototypeAST> ProtoAST, std::string ExternalName) :
    //     ProtoAST(ProtoAST), ExternalName(ExternalName) {}
        
    //     const std::shared_ptr<PrototypeAST> getProtoAST() const { return ProtoAST; }
        
    //     std::string getExternalName() const { return ExternalName; }
        
    //     llvm::Function *codegen();
    // };
    
	namespace eval {

        extern llvm::LLVMContext TheContext;
        extern llvm::IRBuilder<> Builder;
        extern std::unique_ptr<llvm::Module> TheModule;
        extern std::unique_ptr<llvm::legacy::FunctionPassManager> TheFPM;
        extern std::unique_ptr<llvm::orc::KaleidoscopeJIT> TheJIT;
        
        namespace types {
            
            extern llvm::IntegerType *Int1Ty;
            extern llvm::IntegerType *Int8Ty;
            extern llvm::IntegerType *Int32Ty;
            extern llvm::IntegerType *Int64Ty;
            
            extern llvm::IntegerType *CharTy;
            extern llvm::IntegerType *SizeTTy;
            
            extern llvm::PointerType *VoidPtrTy;
            extern llvm::PointerType *CharPtrTy;
            extern llvm::PointerType *Int8PtrTy;
            
            extern llvm::PointerType *FiniteWordPtrTy;
            extern llvm::PointerType *RationalWordPtrTy;
            extern llvm::PointerType *ListPtrTy;
            extern llvm::PointerType *BuiltinSymbolPtrTy;
            extern llvm::PointerType *BuiltinFunctionPtrTy;
            
            extern llvm::StructType *TuppenceValueTy;
            extern llvm::PointerType *TuppenceValuePtrTy;
            extern llvm::PointerType *TuppenceValuePtrPtrTy;
        }
        
        extern std::map<std::string, llvm::FunctionType *> RuntimeFunctionTypes;
        
        extern std::map<std::string, TuppenceValue **> NamedValues;
        
        void InitializeTypes();
        
        void InitializeRuntimeFunctions();

        void InitializeNamedValues();
        
        void InitializeModuleAndPassManager();
        
	} // end namespace eval
    
} // end namespace tuppence
