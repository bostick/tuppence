//===------ FiniteWord.cpp ------------------------------------------------===//
//
// The Tuppence Programming Language
//
// This file is distributed under the MIT Open Source License.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

#include "../common/BuiltinSymbol.h"


struct BuiltinSymbol {
    const char *Name;
    
    BuiltinSymbol(const char *Name) : Name(Name) {}
};



BuiltinSymbol *BuiltinSymbol_INFINITY;


void BuiltinSymbol_initialize() {
    
    BuiltinSymbol_INFINITY = BuiltinSymbol_createFromString("infinity");
}



BuiltinSymbol *BuiltinSymbol_createFromString(const char* Name) {
    auto NameDupe = strdup(Name);
    auto Sym = new BuiltinSymbol(NameDupe);
    return Sym;
}

BuiltinSymbol *BuiltinSymbol_createFromBuiltinSymbol(BuiltinSymbol *sym) {
    
    auto NameDupe = strdup(sym->Name);
    
    auto Sym = new BuiltinSymbol(NameDupe);
    return Sym;
}



int32_t BuiltinSymbol_newString(BuiltinSymbol *sym, char **str) {
    
    auto len = strlen(sym->Name);
    auto printable = new char[len+2+1];
    strncpy(printable+1, sym->Name, len);
    printable[0] = '<';
    printable[len+1] = '>';
    printable[len+1+1] = '\0';
    
    *str = printable;
    
    return 0;
}




const char *BuiltinSymbol_getName(BuiltinSymbol *sym) {
    return sym->Name;
}



bool BuiltinSymbol_same(BuiltinSymbol *sym1, BuiltinSymbol *sym2) {
    return strcmp(sym1->Name, sym2->Name) == 0;
}

