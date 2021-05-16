//===------ Interpreter.h --------------------------------------------------------===//
//
// The Tuppence Programming Language
//
// This file is distributed under the MIT Open Source License.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

#pragma once

#include "../lib/Parser.h"

#include <string>

namespace tuppence {
    
    const int COMMANDLINE_INTERACTIVEBIT = 0x1;
    const int COMMANDLINE_JUPYTERBIT = 0x2;
//    const int COMMANDLINE_CODEGENBIT = 0x4;
    
    class Interpreter {
        Parser Parse;
        int flags;
        
    public:
        Interpreter(std::istream& in, int flags);
        
        void HandleDefinition();
        
//        void HandleCommand();
        
        void HandleTopLevelExpression();
        
        void MainLoop();
        
//        const std::shared_ptr<Value> eval();
    };
    
} // end namespace tuppence
