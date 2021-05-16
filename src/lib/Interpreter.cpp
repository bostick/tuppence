//===------ Interpreter.cpp ------------------------------------------------===//
//
// The Tuppence Programming Language
//
// This file is distributed under the MIT Open Source License.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

#include "../lib/Interpreter.h"

#include "../lib/Eval.h"
#include "../lib/KaleidoscopeJIT.h"

#include "../common/AST.h"

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

using namespace tuppence;

typedef TuppenceValue *(*TopLevelFunction)();


Interpreter::Interpreter(std::istream& in, int flags) :
Parse(Parser(in)), flags(flags) {
    
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
    
    eval::TheJIT = llvm::make_unique<llvm::orc::KaleidoscopeJIT>();
    
    Value_initialize();
    
    eval::InitializeTypes();
    eval::InitializeRuntimeFunctions();
    eval::InitializeNamedValues();
    
};

void Interpreter::HandleDefinition() {
    if (auto TopLevelDefine = Parse.ParseDefinition()) {
        
        eval::InitializeModuleAndPassManager();
        
        auto Func = TopLevelDefine->codegen();
        if (!Func) {
            return;
        }
        
            llvm::errs() << "DUMPING:\n";
            eval::TheModule->dump();
            llvm::errs() << "\n";
            llvm::errs() << "\n";
        
            eval::TheJIT->addModule(std::move(eval::TheModule));
        
            eval::InitializeModuleAndPassManager();
        
    }
    else {
        Parse.throwAwayLine();
    }
}

void Interpreter::HandleTopLevelExpression() {
    
    if (auto TopLevelDefine = Parse.ParseTopLevelExpression()) {
        
        eval::InitializeModuleAndPassManager();
        
        auto Func = TopLevelDefine->codegen();
        if (!Func) {
            return;
        }
        
        llvm::errs() << "DUMPING:\n";
        eval::TheModule->dump();
        llvm::errs() << "\n";
        llvm::errs() << "\n";
        
        // JIT the module containing the anonymous expression, keeping a handle so
        // we can free it later.
        auto H = eval::TheJIT->addModule(std::move(eval::TheModule));
//        eval::InitializeModuleAndPassManager();
        
        // Search the JIT for the __anon_expr symbol.
        if (auto ExprSymbol = eval::TheJIT->findSymbol("__anon_expr")) {
        
        // Get the symbol's address and cast it to the right type (takes no
        // arguments, returns a double) so we can call it as a native function.
            
            if (auto AddressOrErr = ExprSymbol.getAddress()) {
                
                auto Address = *AddressOrErr;
                
                auto FP = (TopLevelFunction)(intptr_t)Address;
                
                auto Val = FP();
                
                char *StrRes;
                Value_CreateString(Val, &StrRes);
                fprintf(stdout, "%s\n", StrRes);
                
                // Delete the anonymous expression module from the JIT.
                eval::TheJIT->removeModule(H);
                
            } else {
                return;
            }
            
        } else {
            return;
        }
    }
    else {
        Parse.throwAwayLine();
    }
}

// top ::= definition | external | expression | ';'
void Interpreter::MainLoop() {
    while (1) {
        if ((flags & COMMANDLINE_INTERACTIVEBIT) == COMMANDLINE_INTERACTIVEBIT) {
            llvm::outs() << ">>> ";
        }
        Parse.readNextToken();
        auto tok = Parse.getCurrentToken();
        switch (tok) {
            case tok_eof:
            case '\n':
                break;
            case tok_error:
                Parse.throwAwayLine();
                break;
            case tok_define:
                HandleDefinition();
                break;
            default:
                HandleTopLevelExpression();
                break;
        }
        
        if (Parse.getCurrentToken() == tok_eof) {
            std::exit(EXIT_SUCCESS);
        }
    }
}
