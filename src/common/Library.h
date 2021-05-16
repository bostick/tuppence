
#pragma once

#include "TuppenceValue.h"

extern "C" RUNTIME_API {
    
    
    TuppenceValue *Library_print(TuppenceValue **Args, size_t Count);
    TuppenceValue *Library_exit(TuppenceValue **Args, size_t Count);
//    TuppenceValue *dump(TuppenceValue **Args, size_t Count);
    TuppenceValue *Library_rationalize(TuppenceValue **Args, size_t Count);
    TuppenceValue *Library_size(TuppenceValue **Args, size_t Count);
    TuppenceValue *Library_period(TuppenceValue **Args, size_t Count);
    TuppenceValue *Library_transient(TuppenceValue **Args, size_t Count);
    TuppenceValue *Library_numerator(TuppenceValue **Args, size_t Count);
    TuppenceValue *Library_denominator(TuppenceValue **Args, size_t Count);
}
