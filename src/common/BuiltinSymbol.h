//===------ FiniteWord.h - FiniteWord class definition --------------------===//
//
// The Tuppence Programming Language
//
// This file is distributed under the MIT Open Source License.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

#pragma once

//#include "finiteword/FiniteWordImpl.h"
// #include "../common/FiniteWordImpl.h"
#include "TuppenceValue.h"

// #include "common/TuppenceValue.h"

#include <memory>
#include <string>

#include <cstddef> // for size_t


#ifdef _WIN32
#    if RUNTIME_DLL
#    define RUNTIME_API __declspec(dllexport)
#    else
#    define RUNTIME_API __declspec(dllimport)
#endif
#else
#define RUNTIME_API
#endif

extern "C" RUNTIME_API {

    void BuiltinSymbol_initialize();
    
    BuiltinSymbol *BuiltinSymbol_createFromString(const char*);
    
    BuiltinSymbol *BuiltinSymbol_createFromBuiltinSymbol(BuiltinSymbol *);
    
    
    int32_t BuiltinSymbol_newString(BuiltinSymbol *sym, char **str);
    
    
    const char *BuiltinSymbol_getName(BuiltinSymbol *);
    
    
    bool BuiltinSymbol_same(BuiltinSymbol *sym1, BuiltinSymbol *sym2);
    
    
    extern BuiltinSymbol *BuiltinSymbol_INFINITY;
}
