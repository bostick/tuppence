
#pragma once

#include "TuppenceValue.h"

extern "C" RUNTIME_API {
    
    TuppenceValue *Runtime_dispatch(TuppenceValue *Callee, TuppenceValue **Args, size_t Count);
    
}
