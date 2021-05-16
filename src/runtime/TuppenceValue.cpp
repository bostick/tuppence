//===------ Runtime.cpp ------------------------------------------------===//
//
// The Tuppence Programming Language
//
// This file is distributed under the MIT Open Source License.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

#include "../common/TuppenceValue.h"

#include "../common/Lexer.h"
#include "../common/List.h"
#include "../common/RationalWord.h"
// #include "../tuppence/Logger.h"
// #include "../tuppence/Value.h"
// #include "FiniteWordImpl.h"
// #include "FiniteWord.h"
// #include "List.h
#include "../common/FiniteWord.h"
#include "../common/BuiltinSymbol.h"
#include "../common/BuiltinFunction.h"

#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

#include <vector>
#include <string.h>




TuppenceValue *TuppenceValue_EMPTYWORD;
TuppenceValue *TuppenceValue_ZERO_1BIT;
TuppenceValue *TuppenceValue_ONE_1BIT;
TuppenceValue *TuppenceValue_ONE;
TuppenceValue *TuppenceValue_ZERO;
TuppenceValue *TuppenceValue_MINUS_ONE;
TuppenceValue *TuppenceValue_EMPTYLIST;

TuppenceValue *TuppenceValue_INFINITY;

TuppenceValue *TuppenceValue_PRINT;
TuppenceValue *TuppenceValue_EXIT;
TuppenceValue *TuppenceValue_RATIONALIZE;
TuppenceValue *TuppenceValue_SIZE;
TuppenceValue *TuppenceValue_TRANSIENT;
TuppenceValue *TuppenceValue_PERIOD;
TuppenceValue *TuppenceValue_NUMERATOR;
TuppenceValue *TuppenceValue_DENOMINATOR;

void Value_initialize() {
    FiniteWord_initialize();
    RationalWord_initialize();

    List_initialize();
    
    BuiltinSymbol_initialize();
    BuiltinFunction_initialize();
    
    TuppenceValue_EMPTYWORD = Value_createFromFiniteWord(FiniteWord_EMPTY);
    TuppenceValue_ZERO_1BIT = Value_createFromFiniteWord(FiniteWord_ZERO_1BIT);
    TuppenceValue_ONE_1BIT = Value_createFromFiniteWord(FiniteWord_ONE_1BIT);
    TuppenceValue_ONE = Value_createFromRationalWord(RationalWord_ONE);
    TuppenceValue_ZERO = Value_createFromRationalWord(RationalWord_ZERO);
    TuppenceValue_MINUS_ONE = Value_createFromRationalWord(RationalWord_MINUS_ONE);
    TuppenceValue_EMPTYLIST = Value_createFromList(List_EMPTY);
    
    TuppenceValue_INFINITY = Value_createFromBuiltinSymbol(BuiltinSymbol_INFINITY);
    
    TuppenceValue_PRINT = Value_createFromBuiltinFunction(BuiltinFunction_PRINT);
    TuppenceValue_EXIT = Value_createFromBuiltinFunction(BuiltinFunction_EXIT);
    TuppenceValue_RATIONALIZE = Value_createFromBuiltinFunction(BuiltinFunction_RATIONALIZE);
    TuppenceValue_SIZE = Value_createFromBuiltinFunction(BuiltinFunction_SIZE);
    TuppenceValue_TRANSIENT = Value_createFromBuiltinFunction(BuiltinFunction_TRANSIENT);
    TuppenceValue_PERIOD = Value_createFromBuiltinFunction(BuiltinFunction_PERIOD);
    TuppenceValue_NUMERATOR = Value_createFromBuiltinFunction(BuiltinFunction_NUMERATOR);
    TuppenceValue_DENOMINATOR = Value_createFromBuiltinFunction(BuiltinFunction_DENOMINATOR);
}



//TuppenceValue *Value_CreateBuiltinFunction(tuppence::PrototypeAST *Proto) {
//    copy XXX;
//    TuppenceValue *val = new TuppenceValue;
//    val->tag = TuppenceValueTag_FiniteWord;
//    val->builtin = Proto;
//    return val;
//}

 TuppenceValue *Value_createFromFiniteWord(FiniteWord *word) {
     TuppenceValue *val = new TuppenceValue;
     val->tag = FiniteWordTag;
     val->finite = FiniteWord_createFromFiniteWord(word);
     return val;
 }

 TuppenceValue *Value_createFromError(const char *msg) {
     TuppenceValue *val = new TuppenceValue;
     
     auto len = strlen(msg);
     auto printableMsg = new char[len+2+1];
     
     strncpy(printableMsg+1, msg, len);
     printableMsg[0] = '!';
     printableMsg[len+1] = '!';
     printableMsg[len+1+1] = '\0';
     
     val->tag = ErrorTag;
     val->error = printableMsg;
     return val;
 }

TuppenceValue *Value_createFromList(List *list) {
    TuppenceValue *val = new TuppenceValue;
    val->tag = ListTag;
    val->list = List_createFromList(list);
    return val;
}

TuppenceValue *Value_createFromRationalWord(RationalWord *rat) {
    TuppenceValue *val = new TuppenceValue;
    val->tag = RationalWordTag;
    val->rational = RationalWord_createFromRationalWord(rat);
    return val;
}

TuppenceValue *Value_createFromBuiltinSymbol(BuiltinSymbol *sym) {
    TuppenceValue *val = new TuppenceValue;
    val->tag = BuiltinSymbolTag;
    val->builtinSymbol = BuiltinSymbol_createFromBuiltinSymbol(sym);
    return val;
}

TuppenceValue *Value_createFromBuiltinFunction(BuiltinFunction *F) {
    TuppenceValue *val = new TuppenceValue;
    val->tag = BuiltinFunctionTag;
    val->builtinFunction = BuiltinFunction_createFromBuiltinFunction(F);
    return val;
}

TuppenceValue *Value_createFromValue(TuppenceValue *val) {
    switch (val->tag) {
        case FiniteWordTag:
            return Value_createFromFiniteWord(val->finite);
        case RationalWordTag:
            return Value_createFromRationalWord(val->rational);
        case ListTag:
            return Value_createFromList(val->list);
        case BuiltinFunctionTag:
            return Value_createFromBuiltinFunction(val->builtinFunction);
        default:
            assert(false);
    }
}




int Value_CreateString(TuppenceValue *val, char** str_p) {

    auto tag = val->tag;
    
    switch (tag) {
        case FiniteWordTag:
            FiniteWord_newString(val->finite, str_p);
            break;
        case RationalWordTag:
            RationalWord_newString(val->rational, str_p);
            break;
        case ListTag:
            List_newString(val->list, str_p);
            break;
        case ErrorTag:
            *str_p = val->error;
            break;
        case BuiltinSymbolTag:
            BuiltinSymbol_newString(val->builtinSymbol, str_p);
            break;
        case BuiltinFunctionTag:
            BuiltinFunction_newString(val->builtinFunction, str_p);
            break;
        default:
            llvm_unreachable("fallthrough");
    }
    
    return 0;
}


uint8_t Value_tag(TuppenceValue *Val) {
    return Val->tag;
}

//void Value_dump(TuppenceValue *Val) {
//    char *S;
//    Value_CreateString(Val, &S);
//    llvm::errs() << S << "\n";
//}


// void Value_ReleaseString(TuppenceValue val, char* str) {

//     auto tag = val.tag;
// //    auto payload = val.payload;

//     switch (tag) {
//         case TuppenceValueTag_FiniteWord:
//             FiniteWord_releaseString(str);
//             break;
//         default:
//             return;
//     }
// }




// void Value_ReleaseValue(TuppenceValue val) {

//     auto tag = val.tag;
    
//     switch (tag) {
//         case TuppenceValueTag_FiniteWord:
//             FiniteWord_release(val.finite);
//             break;
//         default:
//             return;
//     }
// }


std::string stringFromToken(char);


TuppenceValue *Value_Unary(char Op, TuppenceValue *OperandValue) {
    
    if (!OperandValue) {
        return nullptr;
    }

    // For arithmetic, bitwise, and structural operations, concatenate all lists together
    if (OperandValue->tag == ListTag) {
        auto ListOperand = OperandValue->list;
        auto Data = List_data(ListOperand);
        auto Size = List_size(ListOperand);
        OperandValue = List_concatenateArgs(Data, Size);
        if (!OperandValue) {
            return nullptr;
        }
    }

    switch (Op) {
    case '-': {
        if (OperandValue->tag == FiniteWordTag) {
            auto FiniteWordOperand = OperandValue->finite;
            return Value_createFromFiniteWord(FiniteWord_minus(FiniteWordOperand));
        }
        else if (OperandValue->tag == RationalWordTag) {
            auto RationalWordOperand = OperandValue->rational;
            return Value_createFromRationalWord(RationalWord_minus(RationalWordOperand));
        }
        
        {
            char *str;
            Value_CreateString(OperandValue, &str);
            return Value_createFromError((std::string("Invalid type for operator ") + stringFromToken(Op) + ": " + str).c_str());
        }
    }
    case tok_dot_dot_dot: {
        if (OperandValue->tag == FiniteWordTag) {
            auto FiniteWordOperand = OperandValue->finite;
            if (FiniteWord_size(FiniteWordOperand) == 0) {
                return Value_createFromFiniteWord(FiniteWord_EMPTY);
            }
            else {
                return Value_createFromRationalWord(RationalWord_createFromPeriodTransient(FiniteWordOperand, FiniteWord_EMPTY));
            }
        }
        
        {
            char *str;
            Value_CreateString(OperandValue, &str);
            return Value_createFromError((std::string("Invalid type for operator ") + stringFromToken(Op) + ": " + str).c_str());
        }
    }
    case '~': {
        if (OperandValue->tag == FiniteWordTag) {
            auto FiniteWordOperand = OperandValue->finite;
            return Value_createFromFiniteWord(FiniteWord_not(FiniteWordOperand));
        }
        else if (OperandValue->tag == RationalWordTag) {
            auto RationalWordOperand = OperandValue->rational;
            return Value_createFromRationalWord(RationalWord_not(RationalWordOperand));
        }
        
        {
            char *str;
            Value_CreateString(OperandValue, &str);
            return Value_createFromError((std::string("Invalid type for operator ") + stringFromToken(Op) + ": " + str).c_str());
        }
    }
    default:
            return Value_createFromError((std::string("Unary operator not found: ") + stringFromToken(Op)).c_str());
    }
}

TuppenceValue *Value_Binary(char Op, TuppenceValue *LL, TuppenceValue *RR) {

    TuppenceValue *LVal;
    TuppenceValue *RVal;
    // For arithmetic and bitwise operations, concatenate all lists together
    {
        if (LL->tag == ListTag) {
            auto ListVal = LL->list;
            auto Data = List_data(ListVal);
            auto Size = List_size(ListVal);
            auto FlatVal = List_concatenateArgs(Data, Size);
            if (FlatVal->tag == ErrorTag) {
                return FlatVal;
            }
            LVal = FlatVal;
        } else {
            LVal = LL;
        }
        
        if (RR->tag == ListTag) {
            auto ListVal = RR->list;
            auto Data = List_data(ListVal);
            auto Size = List_size(ListVal);
            auto FlatVal = List_concatenateArgs(Data, Size);
            if (FlatVal->tag == ErrorTag) {
                return FlatVal;
            }
            RVal = FlatVal;
        } else {
            RVal = RR;
        }
    }
    
    switch (Op) {
        case tok_percent_percent: {
            if (LVal->tag == RationalWordTag) {
                auto RationalWordL = LVal->rational;
                if (RVal->tag == RationalWordTag) {
                    auto RationalWordR = RVal->rational;
                    if (!RationalWord_isNonNegativeInteger(RationalWordR)) {
                        char *str;
                        Value_CreateString(RVal, &str);
                        return Value_createFromError((std::string("Expected non-negative integer on RHS of ") + stringFromToken(Op) + ": " + str).c_str());
                    }
                    auto i = RationalWord_integerValue(RationalWordR);
                    return Value_createFromFiniteWord(RationalWord_residue(RationalWordL, i));
                } else if (RVal->tag == BuiltinSymbolTag) {
                    auto BuiltinSymbolR = RVal->builtinSymbol;
                    if (BuiltinSymbol_same(BuiltinSymbolR, BuiltinSymbol_INFINITY)) {
                        return LVal;
                    }
                }
            }
            else if (LVal->tag == FiniteWordTag) {
                auto FiniteWordL = LVal->finite;
                if (RVal->tag == RationalWordTag) {
                    auto RationalWordR = RVal->rational;
                    if (!RationalWord_isNonNegativeInteger(RationalWordR)) {
                        char *str;
                        Value_CreateString(RVal, &str);
                        return Value_createFromError((std::string("Expected non-negative integer on RHS of ") + stringFromToken(Op) + ": " + str).c_str());
                    }
                    auto i = RationalWord_integerValue(RationalWordR);
                    if (i > FiniteWord_size(FiniteWordL)) {
                        char *Lstr;
                        Value_CreateString(LVal, &Lstr);
                        char *Rstr;
                        Value_CreateString(RVal, &Rstr);
                        return Value_createFromError((std::string("Requested residue is too large: ") + Lstr + " " + Rstr).c_str());
                    }
                    return Value_createFromFiniteWord(FiniteWord_residue(FiniteWordL, i));
                }
                else if (RVal->tag == BuiltinSymbolTag) {
                    auto BuiltinSymbolR = RVal->builtinSymbol;
                    if (BuiltinSymbol_same(BuiltinSymbolR, BuiltinSymbol_INFINITY)) {
                        return LVal;
                    }
                }
            }
            
            {
                char *Lstr;
                Value_CreateString(LVal, &Lstr);
                char *Rstr;
                Value_CreateString(RVal, &Rstr);
                return Value_createFromError((std::string("Incorrect types for ") + stringFromToken(Op) + ": " + Lstr + " " + Rstr).c_str());
            }
        }
        case tok_greater_greater: {
            if (LVal->tag == RationalWordTag) {
                auto RationalWordL = LVal->rational;
                if (RVal->tag == RationalWordTag) {
                    auto RationalWordR = RVal->rational;
                    if (!RationalWord_isNonNegativeInteger(RationalWordR)) {
                        char *Rstr;
                        Value_CreateString(RVal, &Rstr);
                        return Value_createFromError((std::string("Expected non-negative integer on RHS of ") + stringFromToken(Op) + ": " + Rstr).c_str());
                    }
                    auto i = RationalWord_integerValue(RationalWordR);
                    return Value_createFromRationalWord(RationalWord_shiftRight(RationalWordL, i));
                }
            }
            else if (LVal->tag == FiniteWordTag) {
                auto FiniteWordL = LVal->finite;
                if (RVal->tag == RationalWordTag) {
                    auto RationalWordR = RVal->rational;
                    if (!RationalWord_isNonNegativeInteger(RationalWordR)) {
                        char *Rstr;
                        Value_CreateString(RVal, &Rstr);
                        return Value_createFromError((std::string("Expected non-negative integer on RHS of ") + stringFromToken(Op) + ": " + Rstr).c_str());
                    }
                    auto i = RationalWord_integerValue(RationalWordR);
                    if (i > FiniteWord_size(FiniteWordL)) {
                        char *Lstr;
                        Value_CreateString(LVal, &Lstr);
                        char *Rstr;
                        Value_CreateString(RVal, &Rstr);
                        return Value_createFromError((std::string("Requested shift is too large: ") + Lstr + " " + Rstr).c_str());
                    }
                    return Value_createFromFiniteWord(FiniteWord_shiftRight(FiniteWordL, i));
                }
            }
            
            {
                char *Lstr;
                Value_CreateString(LVal, &Lstr);
                char *Rstr;
                Value_CreateString(RVal, &Rstr);
                return Value_createFromError((std::string("Incorrect types for ") + stringFromToken(Op) + ": " + Lstr + " " + Rstr).c_str());
            }
        }
        case tok_greater_percent: {
            if (LVal->tag == RationalWordTag) {
                auto RationalWordL = LVal->rational;
                if (RVal->tag == RationalWordTag) {
                    auto RationalWordR = RVal->rational;
                    if (!RationalWord_isNonNegativeInteger(RationalWordR)) {
                        char *Rstr;
                        Value_CreateString(RVal, &Rstr);
                        return Value_createFromError((std::string("Expected non-negative integer on RHS of ") + stringFromToken(Op) + ": " + Rstr).c_str());
                    }
                    auto i = RationalWord_integerValue(RationalWordR);
                    FiniteWord *Lo;
                    RationalWord *Hi;
                    RationalWord_shiftRightResidue(RationalWordL, i, &Hi, &Lo);
                    std::vector<TuppenceValue *> Vec;
                    Vec.push_back(Value_createFromFiniteWord(Lo));
                    Vec.push_back(Value_createFromRationalWord(Hi));
                    return Value_createFromList(List_createFromVals(&Vec[0], Vec.size()));
                }
            }
            else if (LVal->tag == FiniteWordTag) {
                auto FiniteWordL = LVal->finite;
                if (RVal->tag == RationalWordTag) {
                    auto RationalWordR = RVal->rational;
                    if (!RationalWord_isNonNegativeInteger(RationalWordR)) {
                        char *Rstr;
                        Value_CreateString(RVal, &Rstr);
                        return Value_createFromError((std::string("Expected non-negative integer on RHS of ") + stringFromToken(Op) + ": " + Rstr).c_str());
                    }
                    auto i = RationalWord_integerValue(RationalWordR);
                    if (i > FiniteWord_size(FiniteWordL)) {
                        char *Lstr;
                        Value_CreateString(LVal, &Lstr);
                        char *Rstr;
                        Value_CreateString(RVal, &Rstr);
                        return Value_createFromError((std::string("Requested shift is too large: ") + Lstr + " " + Rstr).c_str());
                    }
                    FiniteWord *Lo;
                    FiniteWord *Hi;
                    FiniteWord_shiftRightResidue(FiniteWordL, i, &Hi, &Lo);
                    
                    std::vector<TuppenceValue *> Vec;
                    Vec.push_back(Value_createFromFiniteWord(Lo));
                    Vec.push_back(Value_createFromFiniteWord(Hi));
                    return Value_createFromList(List_createFromVals(&Vec[0], Vec.size()));
                }
            }
            
            {
                char *Lstr;
                Value_CreateString(LVal, &Lstr);
                char *Rstr;
                Value_CreateString(RVal, &Rstr);
                return Value_createFromError((std::string("Incorrect types for ") + stringFromToken(Op) + ": " + Lstr + " " + Rstr).c_str());
            }
        }
        case '-': {
            if (LVal->tag == FiniteWordTag) {
                auto FiniteWordL = LVal->finite;
                if (RVal->tag == FiniteWordTag) {
                    auto FiniteWordR = RVal->finite;
                    if (FiniteWord_size(FiniteWordL) != FiniteWord_size(FiniteWordR)) {
                        char *Lstr;
                        Value_CreateString(LVal, &Lstr);
                        char *Rstr;
                        Value_CreateString(RVal, &Rstr);
                        return Value_createFromError((std::string("Expected same sizes for ") + stringFromToken(Op) + ": " + Lstr + " " + Rstr).c_str());
                    }
                    if (FiniteWord_size(FiniteWordL) == 0) {
                        char *Lstr;
                        Value_CreateString(LVal, &Lstr);
                        char *Rstr;
                        Value_CreateString(RVal, &Rstr);
                        return Value_createFromError((std::string("Expected positive sizes for ") + stringFromToken(Op) + ": " + Lstr + " " + Rstr).c_str());
                    }
                    
                    
                    std::vector<FiniteWord *> Args;
                    Args.push_back(FiniteWordR);
                    Args.push_back(FiniteWordL);
                    
                    FiniteWord *Lo;
                    FiniteWord *Hi;
                    FiniteWord_arraySubtract(&Args[0], 2, &Hi, &Lo);
                    
                    std::vector<TuppenceValue *> Vals;
                    Vals.push_back(Value_createFromFiniteWord(Lo));
                    Vals.push_back(Value_createFromFiniteWord(Hi));
                    return Value_createFromList(List_createFromVals(&Vals[0], Vals.size()));
                }
                else {
                    char *Lstr;
                    Value_CreateString(LVal, &Lstr);
                    char *Rstr;
                    Value_CreateString(RVal, &Rstr);
                    return Value_createFromError((std::string("Invalid types for ") + stringFromToken(Op) + ": " + Lstr + " " + Rstr).c_str());
                }
            }
            else if (LVal->tag == RationalWordTag) {
                auto RationalWordL = LVal->rational;
                if (RVal->tag == RationalWordTag) {
                    auto RationalWordR = RVal->rational;
                    return Value_createFromRationalWord(RationalWord_subtract(RationalWordL, RationalWordR));
                }
                else {
                    char *Lstr;
                    Value_CreateString(LVal, &Lstr);
                    char *Rstr;
                    Value_CreateString(RVal, &Rstr);
                    return Value_createFromError((std::string("Invalid types for ") + stringFromToken(Op) + ": " + Lstr + " " + Rstr).c_str());
                }
            }
            else {
                char *Lstr;
                Value_CreateString(LVal, &Lstr);
                char *Rstr;
                Value_CreateString(RVal, &Rstr);
                return Value_createFromError((std::string("Invalid types for ") + stringFromToken(Op) + ": " + Lstr + " " + Rstr).c_str());
            }
        }
        case '/': {
            if (LVal->tag == RationalWordTag) {
                auto RationalWordL = LVal->rational;
                if (RVal->tag == RationalWordTag) {
                    auto RationalWordR = RVal->rational;
                    if (FiniteWord_equal(RationalWord_residue(RationalWordR, 1), FiniteWord_ZERO_1BIT)) {
                        char *Rstr;
                        Value_CreateString(RVal, &Rstr);
                        return Value_createFromError((std::string("Divisor cannot have 0 first bit for ") + stringFromToken(Op) + ": " + Rstr).c_str());
                    }
                    return Value_createFromRationalWord(RationalWord_divide(RationalWordL, RationalWordR));
                }
                else {
                    char *Lstr;
                    Value_CreateString(LVal, &Lstr);
                    char *Rstr;
                    Value_CreateString(RVal, &Rstr);
                    return Value_createFromError((std::string("Invalid types for ") + stringFromToken(Op) + ": " + Lstr + " " + Rstr).c_str());
                }
            }
            else {
                char *Lstr;
                Value_CreateString(LVal, &Lstr);
                char *Rstr;
                Value_CreateString(RVal, &Rstr);
                return Value_createFromError((std::string("Invalid types for ") + stringFromToken(Op) + ": " + Lstr + " " + Rstr).c_str());
            }
        }
        case tok_star_star: {
            if (LVal->tag == RationalWordTag) {
                auto RationalWordL = LVal->rational;
                if (RVal->tag == FiniteWordTag) {
                    auto FiniteWordR = RVal->finite;
                    if (!RationalWord_isNonNegativeInteger(RationalWordL)) {
                        char *Lstr;
                        Value_CreateString(LVal, &Lstr);
                        return Value_createFromError((std::string("Expected non-negative integer on LHS of ") + stringFromToken(Op) + ": " + Lstr).c_str());
                    }
                    auto i = RationalWord_integerValue(RationalWordL);
                    return Value_createFromFiniteWord(FiniteWord_createFromRepsWord(i, FiniteWordR));
                }
            } else if (LVal->tag == BuiltinSymbolTag) {
                auto BuiltinSymbolL = LVal->builtinSymbol;
                if (RVal->tag == FiniteWordTag) {
                    auto FiniteWordR = RVal->finite;
                    if (BuiltinSymbol_same(BuiltinSymbolL, BuiltinSymbol_INFINITY)) {
                        return Value_createFromRationalWord(RationalWord_createFromPeriodTransient(FiniteWordR, FiniteWord_EMPTY));
                    }
                }
            }
            
            {
                char *Lstr;
                Value_CreateString(LVal, &Lstr);
                char *Rstr;
                Value_CreateString(RVal, &Rstr);
                return Value_createFromError((std::string("Incorrect types for ") + stringFromToken(Op) + ": " + Lstr + " " + Rstr).c_str());
            }
        }
        case tok_slash_percent: {
            if (LVal->tag == RationalWordTag) {
                auto RationalWordL = LVal->rational;
                if (RVal->tag == RationalWordTag) {
                    auto RationalWordR = RVal->rational;
                    RationalWord *Remainder;
                    RationalWord *Quotient;
                    RationalWord_quotientRemainder(RationalWordL, RationalWordR, &Quotient, &Remainder);
                    
                    std::vector<TuppenceValue *> Vals;
                    Vals.push_back(Value_createFromRationalWord(Remainder));
                    Vals.push_back(Value_createFromRationalWord(Quotient));
                    return Value_createFromList(List_createFromVals(&Vals[0], Vals.size()));
                }
            } else if (LVal->tag == FiniteWordTag) {
                auto FiniteWordL = LVal->finite;
                if (RVal->tag == FiniteWordTag) {
                    auto FiniteWordR = RVal->finite;
                    if (FiniteWord_size(FiniteWordL) != FiniteWord_size(FiniteWordR)) {
                        char *Lstr;
                        Value_CreateString(LVal, &Lstr);
                        char *Rstr;
                        Value_CreateString(RVal, &Rstr);
                        return Value_createFromError((std::string("Expected same sizes for ") + stringFromToken(Op) + ": " + Lstr + " " + Rstr).c_str());
                    }
                    if (FiniteWord_size(FiniteWordL) == 0) {
                        char *Lstr;
                        Value_CreateString(LVal, &Lstr);
                        char *Rstr;
                        Value_CreateString(RVal, &Rstr);
                        return Value_createFromError((std::string("Expected positive sizes for ") + stringFromToken(Op) + ": " + Lstr + " " + Rstr).c_str());
                    }
                    FiniteWord *Remainder;
                    FiniteWord *Quotient;
                    FiniteWord_udivrem(FiniteWordL, FiniteWordR, &Quotient, &Remainder);
                    
                    std::vector<TuppenceValue *> Vals;
                    Vals.push_back(Value_createFromFiniteWord(Remainder));
                    Vals.push_back(Value_createFromFiniteWord(Quotient));
                    return Value_createFromList(List_createFromVals(&Vals[0], Vals.size()));
                }
            }
            
            {
                char *Lstr;
                Value_CreateString(LVal, &Lstr);
                char *Rstr;
                Value_CreateString(RVal, &Rstr);
                return Value_createFromError((std::string("Incorrect types for ") + stringFromToken(Op) + ": " + Lstr + " " + Rstr).c_str());
            }
        }
    default:
        {
            return Value_createFromError((std::string("Binary operator not found: ") + stringFromToken(Op)).c_str());
        }
    }
}


TuppenceValue *Value_Infix(char Op, TuppenceValue **Args, size_t Count) {
    
    std::vector<TuppenceValue *> Vals(Args, Args + Count);
    
    if (Op == tok_equal_equal || Op == tok_bang_equal) {
        // == and != operators are special, if any word values are present, then only consider
        // the first element of any lists
        // if all arguments are lists, then compare element-wise
        // functions are not comparable to anything
        
        // first determine if any are FiniteWords, and the necessary size
        auto finiteWordIsPresent(false);
        auto rationalWordIsPresent(false);
        size_t finiteWordSize(0);
        size_t listSize(0);
        for (auto& Val : Vals) {
            if (Val->tag == FiniteWordTag) {
                finiteWordIsPresent = true;
                // use first size that is encountered
                auto FiniteWordVal = Val->finite;
                finiteWordSize = FiniteWord_size(FiniteWordVal);
            }
            else if (Val->tag == RationalWordTag) {
                rationalWordIsPresent = true;
            }
            else if (Val->tag == ListTag) {
                auto ListVal = Val->list;
                listSize = List_size(ListVal);
            }
        }
        
        // if FiniteWords are present, then check sizes are correct
        // ensure only FiniteWords and RationalWords are present
        if (finiteWordIsPresent) {
            std::vector<FiniteWord *> FiniteWordVals;
            
            for (auto& Val : Vals) {
                if (Val->tag == FiniteWordTag) {
                    auto FiniteWordVal = Val->finite;
                    if (FiniteWord_size(FiniteWordVal) != finiteWordSize) {
                        char *str;
                        FiniteWord_newString(FiniteWordVal, &str);
                        std::string s = std::string(str);
//                        FiniteWord_releaseString(str);
                        return Value_createFromError((std::string("Incorrect size for ") + stringFromToken(Op) + ":" + s).c_str());
                    }
                    FiniteWordVals.push_back(FiniteWordVal);
                }
                else if (Val->tag == ListTag) {
                    auto ListVal = Val->list;
                    auto First = List_get(ListVal, 0);
                    if (First->tag == FiniteWordTag) {
                        auto FiniteWordVal = Val->finite;
                        if (FiniteWord_size(FiniteWordVal) != finiteWordSize) {
                            char *str;
                            FiniteWord_newString(FiniteWordVal, &str);
                            std::string s = std::string(str);
//                            FiniteWord_releaseString(str);
                            return Value_createFromError((std::string("Incorrect size for ") + stringFromToken(Op) + ":" + s).c_str());
                        }
                        FiniteWordVals.push_back(FiniteWordVal);
                    }
                }
                else {
                    char *str;
                    Value_CreateString(Val, &str);
                    std::string s = std::string(str);
//                    Value_ReleaseString(Val, str);
                    return Value_createFromError((std::string("Invalid type for ") + stringFromToken(Op) + ":" + s).c_str());
                }
            }
            
            switch (Op) {
                case tok_equal_equal:
                    if (FiniteWord_arrayEqual(&FiniteWordVals[0], FiniteWordVals.size())) {
                        return Value_createFromFiniteWord(FiniteWord_ONE_1BIT);
                    } else {
                        return Value_createFromFiniteWord(FiniteWord_ZERO_1BIT);
                    }
                case tok_bang_equal:
                    if (FiniteWord_arrayNotEqual(&FiniteWordVals[0], FiniteWordVals.size())) {
                        return Value_createFromFiniteWord(FiniteWord_ONE_1BIT);
                    } else {
                        return Value_createFromFiniteWord(FiniteWord_ZERO_1BIT);
                    }
                default:
                    llvm_unreachable("fallthrough");
            }
        }
        else if (rationalWordIsPresent) {
            std::vector<RationalWord *> RationalWordVals;
            
            // ensure only RationalWords are present
            for (auto& Val : Vals) {
                if (Val->tag == RationalWordTag) {
                    auto RationalWordVal = Val->rational;
                    RationalWordVals.push_back(RationalWordVal);
                }
                else if (Val->tag == ListTag) {
                    auto ListVal = Val->list;
                    auto First = List_get(ListVal, 0);
                    if (First->tag == RationalWordTag) {
                        auto RationalWordVal = First->rational;
                        RationalWordVals.push_back(RationalWordVal);
                    }
                }
                else {
                    char *str;
                    Value_CreateString(Val, &str);
                    std::string s = std::string(str);
//                    Value_ReleaseString(Val, str);
                    return Value_createFromError((std::string("Invalid type for ") + stringFromToken(Op) +":" + s).c_str());
                }
            }
            
            switch (Op) {
                case tok_equal_equal:
                    if (RationalWord_arrayEqual(&RationalWordVals[0], RationalWordVals.size())) {
                        return Value_createFromFiniteWord(FiniteWord_ONE_1BIT);
                    } else {
                        return Value_createFromFiniteWord(FiniteWord_ZERO_1BIT);
                    }
                case tok_bang_equal:
                    if (RationalWord_arrayNotEqual(&RationalWordVals[0], RationalWordVals.size())) {
                        return Value_createFromFiniteWord(FiniteWord_ONE_1BIT);
                    } else {
                        return Value_createFromFiniteWord(FiniteWord_ZERO_1BIT);
                    }
                default:
                    llvm_unreachable("fallthrough");
            }
        }
        else {
            // assume comparing lists element-wise
            std::vector<List *> ListVals;
            
            // ensure only Lists are present
            for (auto& Val : Vals) {
                if (Val->tag == ListTag) {
                    auto ListVal = Val->list;
                    if (List_size(ListVal) != listSize) {
                        char *str;
                        Value_CreateString(Val, &str);
                        std::string s = std::string(str);
//                        Value_ReleaseString(Val, str);
                        return Value_createFromError((std::string("Incorrect list size for ") + stringFromToken(Op) + ":" + s).c_str());
                    }
                    if (!List_isComparable(ListVal)) {
                        char *str;
                        Value_CreateString(Val, &str);
                        std::string s = std::string(str);
//                        Value_ReleaseString(Val, str);
                        return Value_createFromError((std::string("List contains non-comparable object: ") + s).c_str());
                    }
                    ListVals.push_back(ListVal);
                }
                else {
                    char *str;
                    Value_CreateString(Val, &str);
                    std::string s = std::string(str);
//                    Value_ReleaseString(Val, str);
                    return Value_createFromError((std::string("Invalid type for ") + stringFromToken(Op) + ":" + s).c_str());
                }
            }
            
            switch (Op) {
                case tok_equal_equal: return Value_createFromFiniteWord(List_arrayEqual(&ListVals[0], ListVals.size()));
                case tok_bang_equal: return Value_createFromFiniteWord(List_arrayNotEqual(&ListVals[0], ListVals.size()));
                default:
                    llvm_unreachable("fallthrough");
            }
        }
    }
    
    if (Op == '#') {
        
        if (Vals[0]->tag == FiniteWordTag) {
            return List_concatenateArgs(&Vals[0], Vals.size());
        } else if (Vals[0]->tag == ListTag) {
            return Value_createFromList(List_concatenateLists(&Vals[0], Vals.size()));
        } else {
            char *str;
            Value_CreateString(Vals[0], &str);
            std::string s = std::string(str);
            //                    Value_ReleaseString(Val, str);
            return Value_createFromError(std::string("Invalid type: " + s).c_str());
        }
    }
    
    // For arithmetic and bitwise operations, concatenate all lists together
    {
        std::vector<TuppenceValue *> FlattenedVals;
        for (decltype(Vals.size()) i = 0; i < Vals.size(); i++) {
            auto Val = Vals[i];
            if (Val->tag == ListTag) {
                auto ListVal = Val->list;
                auto Data = List_data(ListVal);
                auto Size = List_size(ListVal);
                auto FlatVal = List_concatenateArgs(Data, Size);
                if (FlatVal->tag == ErrorTag) {
                    return FlatVal;
                }
                FlattenedVals.push_back(FlatVal);
            }
            else {
                FlattenedVals.push_back(Val);
            }
        }
        Vals = FlattenedVals;
    }
    
    // Now, only remaining operators are those that cast to either FiniteWord or RationalWord
    
    if (Op == '+' ||
        Op == '*' ||
        Op == '|' ||
        Op == '&' ||
        Op == '^') {
        // first determine if any are FiniteWords, and the necessary size
        auto finiteWordIsPresent(false);
        auto rationalWordIsPresent(false);
        size_t finiteWordSize(0);
        for (auto& Val : Vals) {
            if (Val->tag == FiniteWordTag) {
                auto FiniteWordVal = Val->finite;
                finiteWordIsPresent = true;
                // use first size that is encountered
                finiteWordSize = FiniteWord_size(FiniteWordVal);
            }
            else if (Val->tag == RationalWordTag) {
                rationalWordIsPresent = true;
            }
        }
        
        // if FiniteWords are present, then check sizes are correct
        // ensure only FiniteWords and RationalWords are present
        if (finiteWordIsPresent) {
            std::vector<FiniteWord*> FiniteWordVals;
            
            for (auto& Val : Vals) {
                if (Val->tag == FiniteWordTag) {
                    auto FiniteWordVal = Val->finite;
                    if (FiniteWord_size(FiniteWordVal) != finiteWordSize) {
                        char *str;
                        Value_CreateString(Val, &str);
                        std::string s = std::string(str);
//                        Value_ReleaseString(Val, str);
                        return Value_createFromError(std::string("Expected FiniteWord with size " + std::to_string(finiteWordSize) +
                                        ", but found FiniteWord with size " + std::to_string(FiniteWord_size(FiniteWordVal)) +
                                        ": " + s).c_str());
                    }
                    FiniteWordVals.push_back(FiniteWordVal);
                }
                else if (Val->tag == RationalWordTag) {
                    auto RationalWordVal = Val->rational;
                    auto FiniteWordVal = RationalWord_residue(RationalWordVal, finiteWordSize);
                    FiniteWordVals.push_back(FiniteWordVal);
                }
                else {
                    char *str;
                    Value_CreateString(Val, &str);
                    std::string s = std::string(str);
//                    Value_ReleaseString(Val, str);
                    return Value_createFromError(std::string("Invalid type: " + s).c_str());
                }
            }
            
            switch (Op) {
                case '+': {
                    FiniteWord *Lo;
                    FiniteWord *Hi;
                    FiniteWord_arrayPlus(&FiniteWordVals[0], FiniteWordVals.size(), &Hi, &Lo);
                    std::vector<TuppenceValue *> Vals;
                    Vals.push_back(Value_createFromFiniteWord(Lo));
                    Vals.push_back(Value_createFromFiniteWord(Hi));
                    return Value_createFromList(List_createFromVals(&Vals[0], Vals.size()));
                }
                case '*': {
                    FiniteWord *Lo;
                    FiniteWord *Hi;
                    FiniteWord_arrayTimes(&FiniteWordVals[0], FiniteWordVals.size(), &Hi, &Lo);
                    std::vector<TuppenceValue *> Vals;
                    Vals.push_back(Value_createFromFiniteWord(Lo));
                    Vals.push_back(Value_createFromFiniteWord(Hi));
                    return Value_createFromList(List_createFromVals(&Vals[0], Vals.size()));
                }
                case '|': return Value_createFromFiniteWord(FiniteWord_arrayOr(&FiniteWordVals[0], FiniteWordVals.size()));
                case '&': return Value_createFromFiniteWord(FiniteWord_arrayAnd(&FiniteWordVals[0], FiniteWordVals.size()));
                case '^': return Value_createFromFiniteWord(FiniteWord_arrayXor(&FiniteWordVals[0], FiniteWordVals.size()));
                default:
                    llvm_unreachable("fallthrough");
            }
        }
        else if (rationalWordIsPresent) {
            std::vector<RationalWord *> RationalWordVals;
            
            // ensure only RationalWords are present
            for (auto& Val : Vals) {
                if (Val->tag == RationalWordTag) {
                    auto RationalWordVal = Val->rational;
                    RationalWordVals.push_back(RationalWordVal);
                }
                else {
                    char *str;
                    Value_CreateString(Val, &str);
                    std::string s = std::string(str);
//                    Value_ReleaseString(Val, str);
                    return Value_createFromError(std::string("Invalid type: " + s).c_str());
                }
            }
            
            switch (Op) {
                case '+': return Value_createFromRationalWord(RationalWord_arrayPlus(&RationalWordVals[0], RationalWordVals.size()));
                case '*': return Value_createFromRationalWord(RationalWord_arrayTimes(&RationalWordVals[0], RationalWordVals.size()));
                case '|': return Value_createFromRationalWord(RationalWord_arrayOr(&RationalWordVals[0], RationalWordVals.size()));
                case '&': return Value_createFromRationalWord(RationalWord_arrayAnd(&RationalWordVals[0], RationalWordVals.size()));
                case '^': return Value_createFromRationalWord(RationalWord_arrayXor(&RationalWordVals[0], RationalWordVals.size()));
                default:
                    llvm_unreachable("fallthrough");
            }
        }
        else {
            return Value_createFromError(std::string("Cannot operate on values").c_str());
        }
    }
    
    return Value_createFromError((std::string("Infix operator not found: ") + stringFromToken(Op)).c_str());

    
    

}




std::string stringFromToken(char Op) {
    switch (Op) {
            
            // properly escape for printing
        case '\n': return "'\\n'";
            
        case tok_eof: return "tok_eof";
        case tok_error: return "tok_error";
            
        case tok_define: return "tok_define";
            
        case tok_identifier: return "tok_identifier";
        case tok_integerword: return "tok_integerword";
        case tok_finiteword: return "tok_finiteword";
            
        case tok_if: return "tok_if";
        case tok_else: return "tok_else";
            //        case tok_for: return "tok_for";
        case tok_while: return "tok_while";
            
        case tok_equal_equal: return "==";
        case tok_percent_percent: return "%%";
        case tok_greater_greater: return ">>";
        case tok_greater_percent: return ">%";
        case tok_dot_dot_dot: return "...";
        case tok_bang_equal: return "!=";
        case tok_star_star: return "**";
        case tok_slash_percent: return "/%";
            
        default:
            if (Op < 0) {
                llvm::errs() << "Add this token: " << static_cast<int>(Op) << "\n";
                assert(false && "Add this token");
            }
            return std::string("'") + Op + "'";
    }
}


