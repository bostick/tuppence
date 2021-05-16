//===------ RationalWord.cpp ----------------------------------------------===//
//
// The Tuppence Programming Language
//
// This file is distributed under the MIT Open Source License.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

// #include "tuppence/RationalWord.h"
#include "../common/RationalWord.h"

#include "../common/FiniteWord.h"
#include "../common/TuppenceMath.h"
#include "TuppenceConfig.h"
#include "../common/Logger.h"
// #include "tuppence/Logger.h"
// #include "runtime/Runtime.h"
// #include "TuppenceConfig.h"

#include <algorithm> // for std::find
#include <cassert>
//#include <sstream>
#include <vector>


int32_t decimalString(RationalWord *word, char **str);

void calculateFraction(RationalWord *word, RationalWord **Numerator, RationalWord **Denominator);


struct RationalWord {
    
    FiniteWord *period;
    FiniteWord *transient;
    
    RationalWord(FiniteWord *period, FiniteWord *transient) :
    period(period),
    transient(transient) {}
    
    // bitwise operations
    //

//    const RationalWord operator~() const;
//    const RationalWord operator|(RationalWord) const;
//    const RationalWord operator&(RationalWord) const;
//    const RationalWord operator^(RationalWord) const;
};

// namespace tuppence {
    
    // namespace rationalword {
        
//        const RationalWord ZERO = RationalWord::FactoryPeriodTransient(tuppence::finiteword::ZERO_1BIT, tuppence::finiteword::EMPTY);
//        const RationalWord ONE = RationalWord::FactoryPeriodTransient(tuppence::finiteword::ZERO_1BIT, tuppence::finiteword::ONE_1BIT);
//        const RationalWord MINUS_ONE = RationalWord::FactoryPeriodTransient(tuppence::finiteword::ONE_1BIT, tuppence::finiteword::EMPTY);
    // }
// }




// using namespace tuppence;
void windupTransient(FiniteWord **period, FiniteWord **transient);
void reduce(FiniteWord **period, FiniteWord **transient);




RationalWord *RationalWord_ONE;
RationalWord *RationalWord_ZERO;
RationalWord *RationalWord_MINUS_ONE;

void RationalWord_initialize() {
    RationalWord_ONE = RationalWord_createFromPeriodTransient(FiniteWord_ZERO_1BIT, FiniteWord_ONE_1BIT);
    RationalWord_ZERO = RationalWord_createFromPeriodTransient(FiniteWord_ZERO_1BIT, FiniteWord_EMPTY);
    RationalWord_MINUS_ONE = RationalWord_createFromPeriodTransient(FiniteWord_ONE_1BIT, FiniteWord_EMPTY);    
}



// Default constructor
//RationalWord::RationalWord() :
//Value(VK_RationalWord), period(finiteword::ZERO_1BIT), transient() {}

//RationalWord::RationalWord(FiniteWord Period, FiniteWord Transient) :
//Value(VK_RationalWord), period(Period), transient(Transient) {
//    assert(isReduced(period, transient) && "not reduced!");
//}

//const RationalWord RationalWord::FactoryPeriodTransient(FiniteWord period, FiniteWord transient) {
//    reduce(period, transient);
//    return RationalWord(period, transient);
//}
RationalWord *RationalWord_createFromPeriodTransient(FiniteWord *period, FiniteWord *transient) {
    auto newPeriod = FiniteWord_createFromFiniteWord(period);
    auto newTransient = FiniteWord_createFromFiniteWord(transient);
    reduce(&newPeriod, &newTransient);
    auto w = new RationalWord(newPeriod, newTransient);
    return w;
}

RationalWord *RationalWord_createFromRationalWord(RationalWord *orig) {
    auto newPeriod = FiniteWord_createFromFiniteWord(orig->period);
    auto newTransient = FiniteWord_createFromFiniteWord(orig->transient);
    reduce(&newPeriod, &newTransient);
    auto w = new RationalWord(newPeriod, newTransient);
    return w;
}

RationalWord *RationalWord_createFromDecimalString(const char *StrVal) {
    std::string test = StrVal;
    assert(!test.empty() && "string is empty");
    assert((test.size() == 1 || test[0] != '0') && "string is padded with 0");
    auto Transient = FiniteWord_createFromDecimalString(test.size(), StrVal);
    return RationalWord_createFromPeriodTransient(FiniteWord_ZERO_1BIT, Transient);
}

RationalWord *RationalWord_createFromVal(size_t numBits, uint64_t val, bool nonNegative) {
    auto newPeriod = FiniteWord_createFromBool(!nonNegative);
    auto newTransient = FiniteWord_createFromVal(numBits, val);
    reduce(&newPeriod, &newTransient);
    auto w = new RationalWord(newPeriod, newTransient);
    return w;
}

int32_t RationalWord_newString(RationalWord *word, char **str) {
    
    return decimalString(word, str);
}




FiniteWord *RationalWord_period(RationalWord *rat) {
    return rat->period;
}

FiniteWord *RationalWord_transient(RationalWord *rat) {
    return rat->transient;
}



//const RationalWord RationalWord::FactorySizeT(size_t Val) {
//    auto Transient = FiniteWord::FactorySizeT(Val);
//    return RationalWord::FactoryPeriodTransient(finiteword::ZERO_1BIT, Transient);
//}

// Copy constructor
//RationalWord::RationalWord(const RationalWord& other) :
//Value(VK_RationalWord), period(other.period), transient(other.transient) {}

// Copy assignment operator
//RationalWord& RationalWord::operator=(const RationalWord& rhs) {
//    period = rhs.period;
//    transient = rhs.transient;
//    return *this;
//}

// Move constructor
//RationalWord::RationalWord(RationalWord&& other) :
//Value(VK_RationalWord), period(other.period), transient(other.transient) {
//    // nothing to clear from other
//}

// Move assignment operator
//RationalWord& RationalWord::operator=(RationalWord&& other) {
//    period = other.period;
//    transient = other.transient;
//    // nothing to clear from other
//    return *this;
//}

void calculateFraction(RationalWord *word, RationalWord **Numerator, RationalWord **Denominator) {
    if (FiniteWord_size(word->transient) > 0) {
        auto z = FiniteWord_concatenate(FiniteWord_ONE_1BIT, FiniteWord_createFromRepsWord(FiniteWord_size(word->transient), FiniteWord_ZERO_1BIT));
        auto xExtended = FiniteWord_concatenate(FiniteWord_concatenate(FiniteWord_ZERO_1BIT, FiniteWord_createFromRepsWord(FiniteWord_size(word->transient), FiniteWord_ZERO_1BIT)), word->period);
        auto zExtended = FiniteWord_concatenate(FiniteWord_createFromRepsWord(FiniteWord_size(word->period), FiniteWord_ZERO_1BIT), z);
        auto xz = FiniteWord_multiply(xExtended, zExtended);
        if (FiniteWord_size(word->period) > 1) {
            auto w = FiniteWord_createFromRepsWord(FiniteWord_size(word->period), FiniteWord_ONE_1BIT);
            auto wExtended = FiniteWord_concatenate(FiniteWord_concatenate(FiniteWord_ZERO_1BIT, FiniteWord_createFromRepsWord(FiniteWord_size(word->transient), FiniteWord_ZERO_1BIT)), w);
            auto yExtended = FiniteWord_concatenate(FiniteWord_concatenate(FiniteWord_ZERO_1BIT, FiniteWord_createFromRepsWord(FiniteWord_size(word->period), FiniteWord_ZERO_1BIT)), word->transient);
            auto wy = FiniteWord_multiply(wExtended, yExtended);
            if (FiniteWord_ugt(wy, xz)) {
                auto wy_xz = FiniteWord_subtract(wy, xz);
                auto divisor = FiniteWord_gcd(wy_xz, wExtended);
                auto wy_xzReduced = FiniteWord_udiv(wy_xz, divisor);
                auto wReduced = FiniteWord_udiv(wExtended, divisor);

                *Denominator = RationalWord_createFromPeriodTransient(FiniteWord_ZERO_1BIT, wReduced);
                *Numerator = RationalWord_createFromPeriodTransient(FiniteWord_ZERO_1BIT, wy_xzReduced);
            }
            else {
                auto xz_wy = FiniteWord_subtract(xz, wy);
                auto divisor = FiniteWord_gcd(xz_wy, wExtended);
                auto xz_wyReduced = FiniteWord_udiv(xz_wy, divisor);
                auto wReduced = FiniteWord_udiv(wExtended, divisor);

                *Denominator = RationalWord_createFromPeriodTransient(FiniteWord_ZERO_1BIT, wReduced);
                *Numerator = RationalWord_minus(RationalWord_createFromPeriodTransient(FiniteWord_ZERO_1BIT, xz_wyReduced));
            }
        }
        else {
            auto yExtended = FiniteWord_concatenate(FiniteWord_concatenate(FiniteWord_ZERO_1BIT, FiniteWord_createFromRepsWord(FiniteWord_size(word->period), FiniteWord_ZERO_1BIT)), word->transient);
            if (FiniteWord_ugt(yExtended, xz)) {

                *Denominator = RationalWord_ONE;
                *Numerator = RationalWord_createFromPeriodTransient(FiniteWord_ZERO_1BIT, FiniteWord_subtract(yExtended, xz));
            }
            else {

                *Denominator = RationalWord_ONE;
                *Numerator = RationalWord_minus(RationalWord_createFromPeriodTransient(FiniteWord_ZERO_1BIT, FiniteWord_subtract(xz, yExtended)));
            }
        }
    }
    else {
        if (FiniteWord_size(word->period) > 1) {
            auto w = FiniteWord_createFromRepsWord(FiniteWord_size(word->period), FiniteWord_ONE_1BIT);

            auto divisor = FiniteWord_gcd(word->period, w);
            auto xReduced = FiniteWord_udiv(word->period, divisor);
            auto wReduced = FiniteWord_udiv(w, divisor);

            *Denominator = RationalWord_createFromPeriodTransient(FiniteWord_ZERO_1BIT, wReduced);
            *Numerator = RationalWord_minus(RationalWord_createFromPeriodTransient(FiniteWord_ZERO_1BIT, xReduced));
        }
        else if (word->period == FiniteWord_ONE_1BIT) {
            *Denominator = RationalWord_ONE;
            *Numerator = RationalWord_MINUS_ONE;
        }
        else {
            *Denominator = RationalWord_ONE;
            *Numerator = RationalWord_ZERO;
        }
    }
}

RationalWord *RationalWord_numerator(RationalWord *rat) {
    RationalWord *Numerator;
    RationalWord *Denominator;
    calculateFraction(rat, &Numerator, &Denominator);
    return Numerator;
}

RationalWord *RationalWord_denominator(RationalWord *rat) {
    RationalWord *Numerator;
    RationalWord *Denominator;
    calculateFraction(rat, &Numerator, &Denominator);
    return Denominator;
}




int32_t decimalString(RationalWord *word, char **str) {
    if (RationalWord_isNegativeInteger(word)) {
        return FiniteWord_newSignedDecimalString(FiniteWord_concatenate(FiniteWord_ONE_1BIT, word->transient), str);
    }
    if (RationalWord_isNonNegativeInteger(word)) {
        return FiniteWord_newDecimalString(word->transient, str);
    }
    RationalWord *Denominator;
    RationalWord *Numerator;
    calculateFraction(word, &Numerator, &Denominator);
    
    char *numeratorStr;
    char *denominatorStr;
    decimalString(Numerator, &numeratorStr);
    decimalString(Denominator, &denominatorStr);
    
    std::string fracStr = std::string(numeratorStr) + "/" + std::string(denominatorStr);
    
    *str = strdup(fracStr.c_str());
    
    return 0;
}

//const std::string RationalWord::string() const {
//    return decimal();
//}

//const std::string RationalWord::typesetString() const {
//    if (isNegativeInteger()) {
//        return finiteword::ONE_1BIT.concatenate(transient).decimalSigned();
//    }
//    if (isNonNegativeInteger()) {
//        return transient.decimal();
//    }
//    RationalWord Denominator;
//    RationalWord Numerator;
//    calculateFraction(Numerator, Denominator);
//    return std::string("\\frac{") + Numerator.decimal() + "}{" + Denominator.decimal() + "}";
//}

//const FiniteWord RationalWord::getPeriod() const {
//    return period;
//}

//const FiniteWord RationalWord::getTransient() const {
//    return transient;
//}

bool RationalWord_isNonNegativeInteger(RationalWord *word) {
    return FiniteWord_size(word->period) == 1 && FiniteWord_equal(word->period, FiniteWord_ZERO_1BIT);
}

bool RationalWord_isNegativeInteger(RationalWord *word) {
    return FiniteWord_size(word->period) == 1 && FiniteWord_equal(word->period, FiniteWord_ONE_1BIT);
}

uint64_t RationalWord_integerValue(RationalWord* word) {
    return FiniteWord_getRawData(word->transient);
}

//const RationalWord RationalWord::getNumerator() const {
//    RationalWord Denominator;
//    RationalWord Numerator;
//    calculateFraction(Numerator, Denominator);
//    return Numerator;
//}

//const RationalWord RationalWord::getDenominator() const {
//    RationalWord Denominator;
//    RationalWord Numerator;
//    calculateFraction(Numerator, Denominator);
//    return Denominator;
//}

void windupTransient(FiniteWord **period, FiniteWord **transient) {
    
    auto periodSize = FiniteWord_size(*period);
    auto transientSize = FiniteWord_size(*transient);
    assert(periodSize > 0 && "Period size is 0");
    while (1) {
        periodSize = FiniteWord_size(*period);
        transientSize = FiniteWord_size(*transient);
        auto min = std::min(periodSize, transientSize);
        auto common = 0;
        for (decltype(min) i = 0; i < min; i++) {
            auto periodBit = FiniteWord_getBit(*period, periodSize - 1 - i);
            auto transientBit = FiniteWord_getBit(*transient, transientSize - 1 - i);
            if (periodBit == transientBit) {
                common++;
            }
            else {
                break;
            }
        }

        if (common == 0) {
            break;
        }

        // rotate period to the left
        *period = FiniteWord_rotateLeft(*period, common);

        //remove last bit of transient;
        *transient = FiniteWord_residue(*transient, transientSize - common);
    }
}

void reduce(FiniteWord **period, FiniteWord **transient) {
    assert(FiniteWord_size(*period) > 0 && "Period size is 0");
    FiniteWord_compressPeriod(period);
    windupTransient(period, transient);
}

//bool RationalWord::isReduced(FiniteWord period, FiniteWord transient) {
//    assert(period.size() > 0 && "Period size is 0");
//    // transient is wound up?
//    if (transient.size() > 0) {
//        auto lastPeriodBit = period.get(period.size() - 1);
//        auto lastTransientBit = transient.get(transient.size() - 1);
//        if (lastPeriodBit == lastTransientBit) {
//            return false;
//        }
//    }
//
//    if (!FiniteWord::isPeriodCompressed(period)) {
//        return false;
//    }
//
//    return true;
//}

// extend transient to be size extent, unwind period to accomodate
void extendTransient(FiniteWord **period, FiniteWord **transient, size_t extent) {
    assert(extent >= FiniteWord_size(*transient) && "Extent is too small");
    auto diff = extent - FiniteWord_size(*transient);

    if (diff == 0) {
        return;
    }

    auto fullRepetitions = diff / FiniteWord_size(*period);
    auto leftOver = diff % FiniteWord_size(*period);

    auto NewTransient = FiniteWord_residue(*period, leftOver);
    NewTransient = FiniteWord_concatenate(NewTransient, FiniteWord_createFromRepsWord(fullRepetitions, *period));
    NewTransient = FiniteWord_concatenate(NewTransient, *transient);

    *transient = NewTransient;
    *period = FiniteWord_rotateRight(*period, diff);
}

void alignPeriodAndTransient(FiniteWord **PeriodL, FiniteWord **TransientL,
                            FiniteWord **PeriodR, FiniteWord **TransientR) {

    // Extend transients to match
    if (FiniteWord_size(*TransientL) > FiniteWord_size(*TransientR)) {
        extendTransient(PeriodR, TransientR, FiniteWord_size(*TransientL));
    }
    else if (FiniteWord_size(*TransientL) < FiniteWord_size(*TransientR)) {
        extendTransient(PeriodL, TransientL, FiniteWord_size(*TransientR));
    }

    // Multiply Periods
    auto LCM = Math_lcm(FiniteWord_size(*PeriodL), FiniteWord_size(*PeriodR));
    auto ToMultiplyL = LCM / FiniteWord_size(*PeriodL);
    auto ToMultiplyR = LCM / FiniteWord_size(*PeriodR);
    FiniteWord_multiplyPeriod(PeriodL, ToMultiplyL);
    FiniteWord_multiplyPeriod(PeriodR, ToMultiplyR);
}

//const RationalWord RationalWord::operator~() const {
//    return RationalWord::FactoryPeriodTransient(~period, ~transient);
//}

//
// Structural Operations
//

bool RationalWord_equal(RationalWord *A, RationalWord *B) {
    if (FiniteWord_size(A->period) == FiniteWord_size(B->period) && FiniteWord_size(A->transient) == FiniteWord_size(B->transient) &&
        FiniteWord_equal(A->period, B->period) && FiniteWord_equal(A->transient, B->transient)) {
        return true;
    } else {
        return false;
    }
}

bool RationalWord_notEqual(RationalWord *A, RationalWord *B) {
    if (FiniteWord_size(A->period) != FiniteWord_size(B->period) || FiniteWord_size(A->transient) != FiniteWord_size(B->transient) ||
        FiniteWord_notEqual(A->period, B->period) || FiniteWord_notEqual(A->transient, B->transient)) {
        return true;
    } else {
        return false;
    }
}

bool RationalWord_arrayEqual(RationalWord **ToTest, size_t Count) {
    assert(Count > 1 && "Vals does not contain more than one element");
    std::vector<RationalWord *> Vals(ToTest, ToTest + Count);
    auto Iter = Vals.begin();
    auto First = *Iter;
    Iter++;
    for (; Iter != Vals.end(); ++Iter) {
        // maintain intuitive order
        if (RationalWord_notEqual(*Iter, First)) {
            return false;
        }
    }
    return true;
}

bool RationalWord_arrayNotEqual(RationalWord **ToTest, size_t Count) {
    assert(Count > 1 && "Vals does not contain more than one element");
    std::vector<RationalWord *> Vals(ToTest, ToTest + Count);
    for (auto Iter = Vals.begin(); Iter != Vals.end(); ++Iter) {
        auto First = *Iter;
        auto Iter2 = Iter;
        Iter2++;
        for (; Iter2 != Vals.end(); Iter2++) {
            auto Second = *Iter2;
            // maintain intuitive order
            if (Second == First) {
                return false;
            }
        }
    }
    return true;
}

FiniteWord *RationalWord_residue(RationalWord *word, size_t i) {
    if (i <= FiniteWord_size(word->transient)) {
        return FiniteWord_residue(word->transient, i);
    }

    auto forPeriod = i - FiniteWord_size(word->transient);
    auto fullRepetitions = forPeriod / FiniteWord_size(word->period);
    auto leftOver = forPeriod % FiniteWord_size(word->period);

    return FiniteWord_concatenate(FiniteWord_concatenate(FiniteWord_residue(word->period, leftOver), FiniteWord_createFromRepsWord(fullRepetitions, word->period)), word->transient);
}

RationalWord *RationalWord_shiftRight(RationalWord *word, size_t i) {
    if (i <= FiniteWord_size(word->transient)) {
        auto Shifted = FiniteWord_shiftRight(word->transient, i);
        return RationalWord_createFromPeriodTransient(word->period, Shifted);
    }

    auto forPeriod = i - FiniteWord_size(word->transient);
    auto leftOver = forPeriod % FiniteWord_size(word->period);

    auto NewPeriod = FiniteWord_rotateRight(word->period, leftOver);
    return RationalWord_createFromPeriodTransient(NewPeriod, FiniteWord_EMPTY);
}

// It is possible to pass this is as Hi or Lo, so make sure
// to make a copy first.
void RationalWord_shiftRightResidue(RationalWord *word, size_t i, RationalWord **Hi, FiniteWord **Lo) {
    auto Tmp = RationalWord_createFromRationalWord(word);
    *Hi = RationalWord_shiftRight(Tmp, i);
    *Lo = RationalWord_residue(Tmp, i);
}

RationalWord *RationalWord_concatenate(RationalWord *word, FiniteWord *other) {
   auto T = FiniteWord_concatenate(word->transient, other);
   return RationalWord_createFromPeriodTransient(word->period, T);
}

//
// Bitwise operations
//

RationalWord *RationalWord_not(RationalWord *L) {
    return RationalWord_createFromPeriodTransient(FiniteWord_not(L->period), FiniteWord_not(L->transient));
}

RationalWord *RationalWord_or(RationalWord *A, RationalWord *B) {
    auto PeriodA = A->period;
    auto PeriodB = B->period;
    auto TransientA = A->transient;
    auto TransientB = B->transient;

    alignPeriodAndTransient(&PeriodA, &TransientA, &PeriodB, &TransientB);

    auto Period = FiniteWord_or(PeriodA, PeriodB);
    auto Transient = FiniteWord_or(TransientA, TransientB);

    return RationalWord_createFromPeriodTransient(Period, Transient);
}

RationalWord *RationalWord_and(RationalWord *A, RationalWord *B) {
    auto PeriodA = A->period;
    auto PeriodB = B->period;
    auto TransientA = A->transient;
    auto TransientB = B->transient;
    
    alignPeriodAndTransient(&PeriodA, &TransientA, &PeriodB, &TransientB);
    
    auto Period = FiniteWord_and(PeriodA, PeriodB);
    auto Transient = FiniteWord_and(TransientA, TransientB);
    
    return RationalWord_createFromPeriodTransient(Period, Transient);
}

RationalWord *RationalWord_xor(RationalWord *A, RationalWord *B) {
    auto PeriodA = A->period;
    auto PeriodB = A->period;
    auto TransientA = A->transient;
    auto TransientB = A->transient;
    
    alignPeriodAndTransient(&PeriodA, &TransientA, &PeriodB, &TransientB);
    
    auto Period = FiniteWord_xor(PeriodA, PeriodB);
    auto Transient = FiniteWord_xor(TransientA, TransientB);
    
    return RationalWord_createFromPeriodTransient(Period, Transient);
}

RationalWord *RationalWord_arrayOr(RationalWord **Values, size_t Count) {
    assert(Count > 1 && "Vals does not contain more than one element");
    std::vector<RationalWord *> Vals(Values, Values + Count);
    auto Iter = Vals.begin();
    auto Val = *Iter;
    Iter++;
    for (; Iter != Vals.end(); ++Iter) {
        // maintain intuitive order
        Val = RationalWord_or(*Iter, Val);
    }
    return Val;
}

RationalWord *RationalWord_arrayAnd(RationalWord **Values, size_t Count) {
    assert(Count > 1 && "Vals does not contain more than one element");
    std::vector<RationalWord *> Vals(Values, Values + Count);
    auto Iter = Vals.begin();
    auto Val = *Iter;
    Iter++;
    for (; Iter != Vals.end(); ++Iter) {
        // maintain intuitive order
        Val = RationalWord_and(*Iter, Val);
    }
    return Val;
}

RationalWord *RationalWord_arrayXor(RationalWord **Values, size_t Count) {
    assert(Count > 1 && "Vals does not contain more than one element");
    std::vector<RationalWord *> Vals(Values, Values + Count);
    auto Iter = Vals.begin();
    auto Val = *Iter;
    Iter++;
    for (; Iter != Vals.end(); ++Iter) {
        // maintain intuitive order
        Val = RationalWord_xor(*Iter, Val);
    }
    return Val;
}

//
// Arithmetic Operations
//

RationalWord *RationalWord_minus(RationalWord *word) {
//    static auto ONE = RationalWord::FactoryString("1");
//    return ~(*this) + RationalWord_ONE;
    return RationalWord_plus(RationalWord_not(word), RationalWord_ONE);
}

RationalWord *RationalWord_plus(RationalWord *word, RationalWord *other) {

    auto PeriodA = word->period;
    auto PeriodB = other->period;
    auto TransientA = word->transient;
    auto TransientB = other->transient;

    alignPeriodAndTransient(&PeriodA, &TransientA, &PeriodB, &TransientB);

    FiniteWord *TransientOutput;
    FiniteWord *TransientCarry;

    // maintain intuitive order
    std::vector<FiniteWord *> TransientVals;
    TransientVals.push_back(TransientB);
    TransientVals.push_back(TransientA);
    FiniteWord_arrayPlus(&TransientVals[0], 2, &TransientCarry, &TransientOutput);
    assert(FiniteWord_size(TransientCarry) == 1 && "Adding 2 FiniteWords did not give a Carry of size 1");

    FiniteWord *PeriodOutput;
    FiniteWord *PeriodCarry = FiniteWord_createFromFiniteWord(TransientCarry);
    FiniteWord *PreviousPeriodCarry;
    std::vector<FiniteWord *> PeriodVals;

    FiniteWord *TransientAccumulate = FiniteWord_createFromFiniteWord(TransientOutput);

    do {
        PreviousPeriodCarry = PeriodCarry;

        auto CarryExtended = FiniteWord_concatenate(FiniteWord_createFromRepsWord(FiniteWord_size(PeriodA) - 1, FiniteWord_ZERO_1BIT), PeriodCarry);

        PeriodVals.clear();
        // maintain intuitive order
        PeriodVals.push_back(PeriodB);
        PeriodVals.push_back(PeriodA);
        PeriodVals.push_back(CarryExtended);
        FiniteWord_arrayPlus(&PeriodVals[0], 3, &PeriodCarry, &PeriodOutput);
        assert(FiniteWord_size(PeriodCarry) == 2 && "Adding 3 FiniteWords did not give a Carry of size 2");
        PeriodCarry = FiniteWord_residue(PeriodCarry, 1);

        TransientAccumulate = FiniteWord_concatenate(PeriodOutput, TransientAccumulate);

    } while (FiniteWord_notEqual(PeriodCarry, PreviousPeriodCarry));

    return RationalWord_createFromPeriodTransient(PeriodOutput, TransientAccumulate);
}

RationalWord *RationalWord_subtract(RationalWord *word, RationalWord *other) {
    auto PeriodA = word->period;
    auto PeriodB = FiniteWord_not(other->period);
    auto TransientA = word->transient;
    auto TransientB = FiniteWord_not(other->transient);

    alignPeriodAndTransient(&PeriodA, &TransientA, &PeriodB, &TransientB);

    FiniteWord *TransientOutput;
    FiniteWord *TransientBorrow;

    auto BorrowExtended = FiniteWord_concatenate(FiniteWord_createFromRepsWord(FiniteWord_size(TransientA) - 1, FiniteWord_ZERO_1BIT), FiniteWord_ONE_1BIT);
    std::vector<FiniteWord *> TransientVals;
    TransientVals.push_back(BorrowExtended);
    TransientVals.push_back(TransientB);
    TransientVals.push_back(TransientA);
    FiniteWord_arrayPlus(&TransientVals[0], 3, &TransientBorrow, &TransientOutput);
    assert(FiniteWord_size(TransientBorrow) == 2 && "Adding 3 FiniteWords did not give a Carry of size 2");
    TransientBorrow = FiniteWord_residue(TransientBorrow, 1);

    FiniteWord *PeriodOutput;
    FiniteWord *PeriodBorrow = FiniteWord_createFromFiniteWord(TransientBorrow);
    FiniteWord *PreviousPeriodBorrow;
    std::vector<FiniteWord *> PeriodVals;

    FiniteWord *TransientAccumulate = FiniteWord_createFromFiniteWord(TransientOutput);

    do {
        PreviousPeriodBorrow = PeriodBorrow;

        auto BorrowExtended = FiniteWord_concatenate(FiniteWord_createFromRepsWord(FiniteWord_size(PeriodA) - 1, FiniteWord_ZERO_1BIT), PeriodBorrow);

        PeriodVals.clear();
        PeriodVals.push_back(PeriodA);
        PeriodVals.push_back(PeriodB);
        PeriodVals.push_back(BorrowExtended);
        FiniteWord_arrayPlus(&PeriodVals[0], 3, &PeriodBorrow, &PeriodOutput);
        assert(FiniteWord_size(PeriodBorrow) == 2 && "Adding 3 FiniteWords did not give a Carry of size 2");
        PeriodBorrow = FiniteWord_residue(PeriodBorrow, 1);

        TransientAccumulate = FiniteWord_concatenate(PeriodOutput, TransientAccumulate);

    } while (FiniteWord_notEqual(PeriodBorrow, PreviousPeriodBorrow));

    return RationalWord_createFromPeriodTransient(PeriodOutput, TransientAccumulate);
}

RationalWord *finiteMultiply(RationalWord *A, FiniteWord *B) {
    auto BReversed = FiniteWord_reverse(B);
    RationalWord *Accumulate;
    FiniteWord *Bit;
    while (FiniteWord_size(BReversed) > 0) {
        Accumulate = RationalWord_concatenate(Accumulate, FiniteWord_ZERO_1BIT);
        FiniteWord_shiftRightResidue(BReversed, 1, &BReversed, &Bit);
        if (FiniteWord_equal(Bit, FiniteWord_ONE_1BIT)) {
            Accumulate = RationalWord_plus(Accumulate, A);
        }
    }
    return Accumulate;
}

RationalWord *RationalWord_times(RationalWord *word, RationalWord *other) {

    auto A = word;
    auto B = other;

    auto BTransientProduct = finiteMultiply(A, B->transient);
    auto BPeriodProduct = finiteMultiply(A, B->period);

    auto BTransientSize = FiniteWord_size(B->transient);
    auto BPeriodSize = FiniteWord_size(B->period);

    FiniteWord *Period;
    FiniteWord *Transient;

    RationalWord *Partial;
    RationalWord_shiftRightResidue(BTransientProduct, BTransientSize, &Partial, &Transient);

    std::vector<RationalWord *> PeriodPartials;

    while (1) {
        Partial = RationalWord_plus(BPeriodProduct, Partial);

        auto PeriodStart = std::find_if(PeriodPartials.begin(), PeriodPartials.end(), [Partial](RationalWord *x){ return RationalWord_equal(x, Partial); });
        if (PeriodStart != PeriodPartials.end()) {
            for (auto TransientIter = PeriodPartials.begin(); TransientIter < PeriodStart; TransientIter++) {
                Transient = FiniteWord_concatenate(RationalWord_residue(*TransientIter, BPeriodSize), Transient);
            }
            for (auto PeriodIter = PeriodStart; PeriodIter < PeriodPartials.end(); PeriodIter++) {
                Period = FiniteWord_concatenate(RationalWord_residue(*PeriodIter, BPeriodSize), Period);
            }

            break;
        }

        PeriodPartials.push_back(Partial);
        if (BTransientSize + PeriodPartials.size() >= Tuppence_LOOP_LIMIT) {
            LogWarning(("Loop limit exceeded in RationalWord multiply. Returning truncated result. Loop limit is: " + std::to_string(Tuppence_LOOP_LIMIT)).c_str());
            for (auto TransientIter = PeriodPartials.begin(); TransientIter < PeriodPartials.end(); TransientIter++) {
                Transient = FiniteWord_concatenate(RationalWord_residue(*TransientIter, 1), Transient);
            }
            return RationalWord_createFromPeriodTransient(FiniteWord_ZERO_1BIT, Transient);
        }

        Partial = RationalWord_shiftRight(Partial, BPeriodSize);
    }

    return RationalWord_createFromPeriodTransient(Period, Transient);
}

RationalWord *RationalWord_divide(RationalWord *word, RationalWord *other) {
    assert(FiniteWord_equal(RationalWord_residue(other, 1), FiniteWord_ONE_1BIT) && "Divisor must be odd!");

    const auto A = word;
    const auto B = other;

    auto Partial = A;

    FiniteWord *Transient = FiniteWord_createEmpty();
    FiniteWord *Period = FiniteWord_createEmpty();

    std::vector<RationalWord *> Partials;

    Partials.push_back(Partial);

    while (1) {
        if (FiniteWord_equal(RationalWord_residue(Partial, 1), FiniteWord_ONE_1BIT)) {
            Partial = RationalWord_subtract(Partial, B);
        }
        Partial = RationalWord_shiftRight(Partial, 1);
        
        auto PeriodStart = std::find_if(Partials.begin(), Partials.end(), [Partial](RationalWord *x){ return RationalWord_equal(x, Partial); });
        if (PeriodStart != Partials.end()) {
            for (auto TransientIter = Partials.begin(); TransientIter < PeriodStart; TransientIter++) {
                Transient = FiniteWord_concatenate(RationalWord_residue(*TransientIter, 1), Transient);
            }
            for (auto PeriodIter = PeriodStart; PeriodIter < Partials.end(); PeriodIter++) {
                Period = FiniteWord_concatenate(RationalWord_residue(*PeriodIter, 1), Period);
            }

            break;
        }

        Partials.push_back(Partial);
        if (Partials.size() >= Tuppence_LOOP_LIMIT) {
            LogWarning((std::string("Loop limit exceeded in RationalWord divide. Returning truncated result. Loop limit is: ") + std::to_string(Tuppence_LOOP_LIMIT)).c_str());
            for (auto TransientIter = Partials.begin(); TransientIter < Partials.end(); TransientIter++) {
                Transient = FiniteWord_concatenate(RationalWord_residue(*TransientIter, 1), Transient);
            }
            return RationalWord_createFromPeriodTransient(FiniteWord_ZERO_1BIT, Transient);
        }
    }

    return RationalWord_createFromPeriodTransient(Period, Transient);
}

void RationalWord_quotientRemainder(RationalWord *L, RationalWord *R, RationalWord **Quotient, RationalWord **Remainder) {

    RationalWord *LDenominator;
    RationalWord *LNumerator;
    calculateFraction(L, &LNumerator, &LDenominator);

    auto LNumeratorInt = RationalWord_integerValue(LNumerator);
    auto LDenominatorInt = RationalWord_integerValue(LDenominator);

    RationalWord *RDenominator;
    RationalWord *RNumerator;
    calculateFraction(R, &RNumerator, &RDenominator);

    auto RNumeratorInt = RationalWord_integerValue(RNumerator);
    auto RDenominatorInt = RationalWord_integerValue(RDenominator);

    auto LCM = Math_lcm(LDenominatorInt, RDenominatorInt);
    
    LNumeratorInt = LNumeratorInt * LCM / LDenominatorInt;
    RNumeratorInt = RNumeratorInt * LCM / RDenominatorInt;
    
    auto RemainderTmp = RationalWord_createFromPeriodTransient(FiniteWord_ZERO_1BIT, FiniteWord_createFromVal(64, LNumeratorInt % RNumeratorInt));
    auto LCMWord = RationalWord_createFromPeriodTransient(FiniteWord_ZERO_1BIT, FiniteWord_createFromVal(64, LCM));
    *Remainder = RationalWord_divide(RemainderTmp, LCMWord);
    *Quotient = RationalWord_createFromPeriodTransient(FiniteWord_ZERO_1BIT, FiniteWord_createFromVal(64, LNumeratorInt / RNumeratorInt));
}

RationalWord *RationalWord_arrayPlus(RationalWord **Values, size_t Count) {
    assert(Count > 1 && "Vals does not contain more than one element");
    std::vector<RationalWord *> Vals(Values, Values + Count);
    auto Iter = Vals.begin();
    auto Val = *Iter;
    Iter++;
    for (; Iter != Vals.end(); ++Iter) {
        // maintain intuitive order
        Val = RationalWord_plus(*Iter, Val);
    }
    return Val;
}

//const RationalWord RationalWord::minus(RationalWord L, RationalWord R) {
//    return L - R;
//}

RationalWord *RationalWord_arrayTimes(RationalWord **Values, size_t Count) {
    assert(Count > 1 && "Vals does not contain more than one element");
    std::vector<RationalWord *> Vals(Values, Values + Count);
    auto Iter = Vals.begin();
    auto Val = *Iter;
    Iter++;
    for (; Iter != Vals.end(); ++Iter) {
        // maintain intuitive order
        Val = RationalWord_times(*Iter, Val);
    }
    return Val;
}

//const RationalWord RationalWord::divide(RationalWord L, RationalWord R) {
//    return L / R;
//}

//void RationalWord::integerDivide(RationalWord Dividend, uint64_t Divisor, RationalWord& Quotient, RationalWord& Remainder) {
//    blah;
//}

//
// GTest
//

//void tuppence::PrintTo(const RationalWord& bar, ::std::ostream* os) {
//    *os << bar.string();
//}

