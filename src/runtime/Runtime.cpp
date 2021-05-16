
#include "../common/Runtime.h"

#include "../common/BuiltinFunction.h"

TuppenceValue *Runtime_dispatch(TuppenceValue *Callee, TuppenceValue **Args, size_t Count) {
    
    if (Callee->tag == BuiltinFunctionTag) {
        
        auto BuiltinFunctionCallee = Callee->builtinFunction;
        
        auto Func = BuiltinFunction_getFunctionPointer(BuiltinFunctionCallee);
        
        auto Res = Func(Args, Count);
        
        return Res;
        
    } else {
        return Value_createFromError("expected BuiltinFunction");
    }
}
