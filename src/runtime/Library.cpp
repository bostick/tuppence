
#include "../common/Library.h"

#include "../common/TuppenceValue.h"
#include "../common/FiniteWord.h"
#include "../common/RationalWord.h"
#include "../common/BuiltinSymbol.h"
#include "../common/BuiltinFunction.h"
#include "../common/List.h"

#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

#include <vector>
#include <string.h>



//
// Built-in Functions
//


TuppenceValue *Library_print(TuppenceValue **Args, size_t Count) {

    if (Count >= 2) {
        return Value_createFromError("print takes 0 or 1 args");
    }
    
    if (Count == 0) {
        
        llvm::outs() << "\n";
        
        return Value_createFromFiniteWord(FiniteWord_EMPTY);
    }
    
    auto ArgsCasted = std::vector<TuppenceValue *>(Args, Args + Count);

    auto Val = ArgsCasted[0];
    
    char *StrRes;
    Value_CreateString(Val, &StrRes);
    llvm::outs() << StrRes << "\n";

    return Value_createFromFiniteWord(FiniteWord_EMPTY);
}

//TuppenceValue *dump(TuppenceValue **Args, size_t Count) {
//    // Print out all of the generated code.
//    eval::TheModule->print(llvm::outs(), nullptr);
//    return 0;
//}

//
// exit() exits with code 0
//
// exit(val) exits with code val
//
TuppenceValue *Library_exit(TuppenceValue **Args, size_t Count) {
    
    if (Count >= 2) {
        return Value_createFromError("exit takes 0 or 1 args");
    }
    
    if (Count == 0) {
        
        std::exit(EXIT_SUCCESS);
        
        llvm_unreachable("fallthrough");
    }
    
    auto ArgsCasted = std::vector<TuppenceValue *>(Args, Args + Count);
    
    auto Val = ArgsCasted[0];
    
    if (Val->tag != RationalWordTag) {
        return Value_createFromError("exit takes a nonnegative integer arg");
    }
    
    auto RationalVal = Val->rational;
    
    if (!RationalWord_isNonNegativeInteger(RationalVal)) {
        return Value_createFromError("exit takes a nonnegative integer arg");
    }
    
    auto intVal = RationalWord_integerValue(RationalVal);
    
    std::exit(intVal);
    llvm_unreachable("fallthrough");
}

TuppenceValue *Library_rationalize(TuppenceValue **Args, size_t Count) {

    auto ArgsCasted = std::vector<TuppenceValue *>(Args, Args + Count);

    auto Val = ArgsCasted[0];

    if (Val->tag == RationalWordTag) {
        return Val;
    } else if (Val->tag == FiniteWordTag) {
        auto FiniteWordVal = Val->finite;
        
        if (FiniteWord_size(FiniteWordVal) == 0) {
            return Value_createFromError("Cannot rationalize empty word");
        }

        FiniteWord *Lo;
        FiniteWord *Hi;
        FiniteWord_shiftRightResidue(FiniteWordVal, FiniteWord_size(FiniteWordVal) - 1, &Hi, &Lo);
        auto Best = RationalWord_createFromPeriodTransient(Hi, Lo);

        for (size_t i = 0; i < FiniteWord_size(FiniteWordVal) - 1; i++) {
            FiniteWord_shiftRightResidue(FiniteWordVal, i, &Hi, &Lo);
            auto Res = RationalWord_createFromPeriodTransient(Hi, Lo);
            if (FiniteWord_size(RationalWord_period(Res)) + FiniteWord_size(RationalWord_transient(Res)) < FiniteWord_size(RationalWord_period(Best)) + FiniteWord_size(RationalWord_transient(Best))) {
                Best = Res;
            }
        }

        return Value_createFromRationalWord(Best);

    }
    else {
        char *str;
        Value_CreateString(Val, &str);
        return Value_createFromError((std::string("Expected RationalWord or FiniteWord: ") + str).c_str());
    }
}

//
// size(finite)
//
// size(rational)
//
// size(list)
//
TuppenceValue *Library_size(TuppenceValue **Args, size_t Count) {

    auto ArgsCasted = std::vector<TuppenceValue *>(Args, Args + Count);

    auto Val = ArgsCasted[0];

    if (Val->tag == FiniteWordTag) {
        auto FiniteWordVal = Val->finite;
        
        auto Size = FiniteWord_size(FiniteWordVal);

        auto RationalSize = RationalWord_createFromVal(64, Size, true);

        return Value_createFromRationalWord(RationalSize);

    } else if (Val->tag == RationalWordTag) {

        return TuppenceValue_INFINITY;
        
    } else if (Val->tag == ListTag) {
        auto ListVal = Val->list;

        auto Size = List_size(ListVal);

        auto RationalSize = RationalWord_createFromVal(64, Size, true);

        return Value_createFromRationalWord(RationalSize);
    }
    else {
        char *str;
        Value_CreateString(Val, &str);
        return Value_createFromError((std::string("Expected FiniteWord, RationalWord, or List: ") + str).c_str());
    }
}

TuppenceValue *Library_period(TuppenceValue **Args, size_t Count) {

    auto ArgsCasted = std::vector<TuppenceValue *>(Args, Args + Count);

    auto Val = ArgsCasted[0];

    if (Val->tag == RationalWordTag) {
        
        auto RationalVal = Val->rational;

        auto Period = RationalWord_period(RationalVal);
        
        return Value_createFromFiniteWord(Period);
    } 
    else {
        char *str;
        Value_CreateString(Val, &str);
        return Value_createFromError((std::string("Expected RationalWord: ") + str).c_str());
    }
}

TuppenceValue *Library_transient(TuppenceValue **Args, size_t Count) {

    auto ArgsCasted = std::vector<TuppenceValue *>(Args, Args + Count);

    auto Val = ArgsCasted[0];

    if (Val->tag == RationalWordTag) {
        
        auto RationalVal = Val->rational;

        auto Transient = RationalWord_transient(RationalVal);
        
        return Value_createFromFiniteWord(Transient);
    } 
    else {
        char *str;
        Value_CreateString(Val, &str);
        return Value_createFromError((std::string("Expected RationalWord: ") + str).c_str());
    }
}

TuppenceValue *Library_numerator(TuppenceValue **Args, size_t Count) {

    auto ArgsCasted = std::vector<TuppenceValue *>(Args, Args + Count);

    auto Val = ArgsCasted[0];

    if (Val->tag == RationalWordTag) {
        
        auto RationalVal = Val->rational;

        auto Numerator = RationalWord_numerator(RationalVal);
        
        return Value_createFromRationalWord(Numerator);
    } 
    else {
        char *str;
        Value_CreateString(Val, &str);
        return Value_createFromError((std::string("Expected RationalWord: ") + str).c_str());
    }
}

TuppenceValue *Library_denominator(TuppenceValue **Args, size_t Count) {

    auto ArgsCasted = std::vector<TuppenceValue *>(Args, Args + Count);

    auto Val = ArgsCasted[0];

    if (Val->tag == RationalWordTag) {
        
        auto RationalVal = Val->rational;

        auto Denominator = RationalWord_denominator(RationalVal);
        
        return Value_createFromRationalWord(Denominator);
    } 
    else {
        char *str;
        Value_CreateString(Val, &str);
        return Value_createFromError((std::string("Expected RationalWord: ") + str).c_str());
    }
}

