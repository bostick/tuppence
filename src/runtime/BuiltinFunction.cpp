//===------ FiniteWord.cpp ------------------------------------------------===//
//
// The Tuppence Programming Language
//
// This file is distributed under the MIT Open Source License.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

#include "../common/BuiltinFunction.h"
#include "../common/Library.h"

#include <string.h>

struct BuiltinFunction {
    const char *Name;
//    const char *ExternalName;
    BuiltinFunctionPointer Func;
    
    
    BuiltinFunction(const char *Name, BuiltinFunctionPointer Func) : Name(Name), Func(Func) {}
};

BuiltinFunction *BuiltinFunction_PRINT;
BuiltinFunction *BuiltinFunction_EXIT;
BuiltinFunction *BuiltinFunction_RATIONALIZE;
BuiltinFunction *BuiltinFunction_SIZE;
BuiltinFunction *BuiltinFunction_TRANSIENT;
BuiltinFunction *BuiltinFunction_PERIOD;
BuiltinFunction *BuiltinFunction_NUMERATOR;
BuiltinFunction *BuiltinFunction_DENOMINATOR;

void BuiltinFunction_initialize() {
    BuiltinFunction_PRINT = BuiltinFunction_createFromName("print", Library_print);
    BuiltinFunction_EXIT = BuiltinFunction_createFromName("exit", Library_exit);
    BuiltinFunction_RATIONALIZE = BuiltinFunction_createFromName("rationalize", Library_rationalize);
    BuiltinFunction_SIZE = BuiltinFunction_createFromName("size", Library_size);
    BuiltinFunction_TRANSIENT = BuiltinFunction_createFromName("transient", Library_transient);
    BuiltinFunction_PERIOD = BuiltinFunction_createFromName("period", Library_period);
    BuiltinFunction_NUMERATOR = BuiltinFunction_createFromName("numerator", Library_numerator);
    BuiltinFunction_DENOMINATOR = BuiltinFunction_createFromName("denominator", Library_denominator);
}


BuiltinFunction *BuiltinFunction_createFromName(const char* Name, BuiltinFunctionPointer Func) {
    auto NameDup = strdup(Name);
//    auto ExternalNameDup = strdup(ExternalName);
    auto F = new BuiltinFunction(NameDup, Func);
    return F;
}

BuiltinFunction *BuiltinFunction_createFromBuiltinFunction(BuiltinFunction *BF) {
    auto NameDup = strdup(BF->Name);
//    auto ExternalNameDup = strdup(BF->ExternalName);
    auto Func = BF->Func;
    auto F = new BuiltinFunction(NameDup, Func);
    return F;
}

int32_t BuiltinFunction_newString(BuiltinFunction *F, char **str) {
    
    auto len = strlen(F->Name);
    auto printable = new char[len+2+1];
    strncpy(printable+1, F->Name, len);
    printable[0] = '<';
    printable[len+1] = '>';
    printable[len+1+1] = '\0';
    
    *str = printable;
    
    return 0;
}


const char *BuiltinFunction_getName(BuiltinFunction *BF) {
    return BF->Name;
}


//llvm::Function *BuiltinFunction_getFunctionValue(BuiltinFunction *BF) {
//    return BF->FP;
//}

BuiltinFunctionPointer BuiltinFunction_getFunctionPointer(BuiltinFunction *BF) {
    return BF->Func;
}


