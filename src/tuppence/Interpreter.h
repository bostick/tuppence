//===------ Interpreter.h --------------------------------------------------------===//
//
// The Tuppence Programming Language
//
// This file is distributed under the MIT Open Source License.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

#pragma once

#include "Parser.h"

#include <iostream>
#include <string>

namespace tuppence {
    
    class Interpreter {
        Parser Parse;
        
    public:
        Interpreter(std::istream& in);
        
        void HandleDefinition(bool interactive);
        
        void HandleTopLevelExpression(bool interactive);
        
        void MainLoop(bool interactive);
        
        const std::shared_ptr<Value> eval();
    };
}
