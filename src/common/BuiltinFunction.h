
#pragma once

#include "TuppenceValue.h"
// #include "AST.h"

#include "llvm/IR/IRBuilder.h"

#include <cstdint>

#ifdef _WIN32
#    if RUNTIME_DLL
#    define RUNTIME_API __declspec(dllexport)
#    else
#    define RUNTIME_API __declspec(dllimport)
#endif
#else
#define RUNTIME_API
#endif


typedef TuppenceValue *(*BuiltinFunctionPointer)(TuppenceValue **, size_t);


extern "C" RUNTIME_API {

    void BuiltinFunction_initialize();
    
    BuiltinFunction *BuiltinFunction_createFromName(const char*, BuiltinFunctionPointer);
    
    BuiltinFunction *BuiltinFunction_createFromBuiltinFunction(BuiltinFunction *);
    
    
    int32_t BuiltinFunction_newString(BuiltinFunction *F, char **str);
    
    
    const char *BuiltinFunction_getName(BuiltinFunction *);
    
    BuiltinFunctionPointer BuiltinFunction_getFunctionPointer(BuiltinFunction *);

    
    extern BuiltinFunction *BuiltinFunction_PRINT;
    extern BuiltinFunction *BuiltinFunction_EXIT;
    extern BuiltinFunction *BuiltinFunction_RATIONALIZE;
    extern BuiltinFunction *BuiltinFunction_SIZE;
    extern BuiltinFunction *BuiltinFunction_TRANSIENT;
    extern BuiltinFunction *BuiltinFunction_PERIOD;
    extern BuiltinFunction *BuiltinFunction_NUMERATOR;
    extern BuiltinFunction *BuiltinFunction_DENOMINATOR;
    
}
