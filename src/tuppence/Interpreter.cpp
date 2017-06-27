//===------ Interpreter.cpp ------------------------------------------------===//
//
// The Tuppence Programming Language
//
// This file is distributed under the MIT Open Source License.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

#include "Interpreter.h"

#include "Eval.h"
#include "FiniteWord.h"

#include "llvm/Support/Casting.h"

#include <cstdlib> // for std::exit
#include <iostream>

using namespace tuppence;

Interpreter::Interpreter(std::istream& in) :
Parse(Parser(in)) {
    
    eval::NamedValues["infinity"] = std::make_shared<Symbol>(eval::SYMBOL_INFINITY);
    eval::NamedValues["print"] = std::make_shared<BuiltinFunction>(eval::BUILTIN_PRINT);
    eval::NamedValues["exit"] = std::make_shared<BuiltinFunction>(eval::BUILTIN_EXIT);
    eval::NamedValues["rationalize"] = std::make_shared<BuiltinFunction>(eval::BUILTIN_RATIONALIZE);
};

void Interpreter::HandleDefinition(bool interactive) {
    if (auto FnAST = Parse.ParseDefinition()) {
        auto FnIR = FnAST->eval();
        if (!FnIR) {
            return;
        }
    }
    else {
        Parse.throwAwayLine();
    }
}

void Interpreter::HandleTopLevelExpression(bool interactive) {
    if (auto EAST = Parse.ParseTopLevelExpression()) {
        auto Res = EAST->eval();
        if (!Res) {
            return;
        }
        
        if (interactive) {
            if (auto FiniteWordRes = llvm::dyn_cast<FiniteWord>(Res.get())) {
                if (FiniteWordRes->getSize() == 0) {
                    // Do not print empty
                }
                else {
                    std::cout << Res->string() << "\n";
                }
            }
            else {
                std::cout << Res->string() << "\n";
            }
        }
    }
    else {
        Parse.throwAwayLine();
    }
}

// top ::= definition | external | expression | ';'
void Interpreter::MainLoop(bool interactive) {
    while (1) {
        if (interactive) {
            std::cout << ">>> ";
        }
        Parse.readNextToken();
        auto tok = Parse.getCurrentToken();
        switch (tok) {
            case tok_identifier:
            case tok_rationalword:
            case tok_finiteword:
            case tok_if:
            case tok_for:
            case tok_while:
            case tok_var:
            case '(':
            case '{':
            case '-':
            case tok_dot_dot_dot:
            case '~':
                HandleTopLevelExpression(interactive);
                break;
            case tok_define:
                HandleDefinition(interactive);
                break;
            case tok_eof:
            case '\n':
                break;
            case tok_error:
                Parse.throwAwayLine();
                break;
            default:
                std::cerr << "Unhandled token: " << tok << "\n";
                Parse.throwAwayLine();
                break;
        }
        
        if (Parse.getCurrentToken() == tok_eof) {
            std::exit(EXIT_SUCCESS);
        }
    }
}

const std::shared_ptr<Value> Interpreter::eval() {
    
    Parse.readNextToken();
    auto E = Parse.ParseTopLevelExpression();
    auto Evaled = E->eval();
    return Evaled;
}
