//===------ Method.h -------------------------------------------------------===//
//
// The Tuppence Programming Language
//
// This file is distributed under the MIT Open Source License.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

#pragma once

#include "tuppence/Value.h"

#include <memory>
#include <string>

namespace tuppence {
    
    class Method {
        const std::shared_ptr<PrototypeAST> Proto;
        const std::shared_ptr<Value>(*FunctionPointer)(std::shared_ptr<Value> Obj, std::vector<std::shared_ptr<Value>> Args);
        
    public:
        Method(const std::shared_ptr<PrototypeAST> Proto, const std::shared_ptr<Value>(*FunctionPointer)(std::shared_ptr<Value> Obj, std::vector<std::shared_ptr<Value>> Args)) :
        Proto(Proto), FunctionPointer(FunctionPointer) {}
        
        const std::shared_ptr<PrototypeAST> getProto() const { return Proto; }
        
        const std::shared_ptr<Value> call(std::shared_ptr<Value> Obj, std::vector<std::shared_ptr<Value>> Args) const;
    };
    
} // end namespace tuppence
