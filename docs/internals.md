
# TuppenceValue struct

Tuppence is a dynamically typed language. Values are represented by the TuppenceValue struct.

The TuppenceValue struct is a type tag followed by a union of pointers. The types of the pointers in the union are the primitive value types in the Tuppence language.


```
enum TuppenceValueTag : uint8_t {
    BuiltinFunctionTag,
    BuiltinSymbolTag,
    FiniteWordTag,
    RationalWordTag,
    UserFunctionTag,
    ListTag,
    ErrorTag
};
```

```
struct TuppenceValue {
    TuppenceValueTag tag;
    union {
        BuiltinFunction *builtinFunction;
        BuiltinSymbol *builtinSymbol;
        FiniteWord *finite;
        RationalWord *rational;
        UserFunction *userFunction;
        List *list;
        char *error;
    };
};
```





# Runtime Library

The Runtime Library provides a C API for creating primitives and working with primitives.




# Tuppence Library

The Tuppence Library uses the Runtime Library.






IdentifierTable
std::map<std::string, llvm::Value *> eval::IdentifierTable;

IdentifierTable is map of std::string to llvm::Value* that are TuppenceValues representing either BuiltinSymbol or BuiltinFunction primitives









## Debugging

dump output to example.ll


### generate a control flow graph

```
~/llvm/build/Debug/bin/llvm-as < example.ll | ~/llvm/build/Debug/bin/opt -analyze -dot-cfg

dot -Tpdf cfg.__anon_expr.dot -o cfg.pdf
```






### generate a call graph

```
~/llvm/build/Debug/bin/llvm-as < example.ll | ~/llvm/build/Debug/bin/opt -analyze -dot-callgraph

dot -Tpdf callgraph.dot -o callgraph.pdf
```








