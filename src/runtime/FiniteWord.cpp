//===------ FiniteWord.cpp ------------------------------------------------===//
//
// The Tuppence Programming Language
//
// This file is distributed under the MIT Open Source License.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

#include "../common/FiniteWord.h"

// #include "runtime/Runtime.h"
#include "../common/TuppenceMath.h"

#include "llvm/ADT/APInt.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/raw_ostream.h"

#include <sstream>
#include <vector>


//class FiniteWordImpl : public gc {
struct FiniteWord {
    size_t Size;
    llvm::APInt Val;
    
    FiniteWord() {}
    
//    void* operator new(size_t sz) {
//        llvm::outs() << "operator new" << "\n";
//        //return ::operator new(sz);
//        return GC_malloc(sz);
//    }
    
//    void operator delete(void* p) {
//        llvm::outs() << "operator delete" << "\n";
//        ::operator delete(p);
//    }
    
    FiniteWord(size_t Size, llvm::APInt Val) :
        Size(Size),
        Val(Val) {
//        llvm::outs() << "creating FiniteWordImpl\n";
    }
    
//    ~FiniteWordImpl() {
//        llvm::outs() << "destroying FiniteWordImpl\n";
//    }
    
};

const std::string FiniteWord_bits(FiniteWord *word);



FiniteWord *FiniteWord_EMPTY;
FiniteWord *FiniteWord_ZERO_1BIT;
FiniteWord *FiniteWord_ONE_1BIT;


void FiniteWord_initialize() {
    FiniteWord_EMPTY = FiniteWord_createEmpty();
    FiniteWord_ZERO_1BIT = FiniteWord_createFromBool((bool)0);
    FiniteWord_ONE_1BIT = FiniteWord_createFromBool((bool)1);
}


// private
FiniteWord *FiniteWord_createEmpty() {
    auto w = new FiniteWord(0, llvm::APInt());
    return w;
}

FiniteWord *FiniteWord_createFromBool(bool Bit) {
    //llvm::outs() << "FiniteWordImpl_createFromAPInt\n";
    auto w = FiniteWord_createFromVal(1, (uint64_t)Bit);
//    GC_register_finalizer(w, finalize, nullptr, nullptr, nullptr);
    return w;
}

FiniteWord *FiniteWord_createFromAPInt(size_t Size, llvm::APInt Init) {
    //llvm::outs() << "FiniteWordImpl_createFromAPInt\n";
    
    assert(Size != 0 && Size == Init.getBitWidth());
    
    auto w = new FiniteWord(Size, Init);
//    GC_register_finalizer(w, finalize, nullptr, nullptr, nullptr);
    return w;
}

FiniteWord *FiniteWord_createFromBinaryString(size_t Size, const char *str) {
    //llvm::outs() << "creating FiniteWordImpl: " << str << "\n";
    
    assert(Size != 0 && Size == strlen(str));
    
    llvm::StringRef text(str);
    auto Val = llvm::APInt(Size, text, 2);
    return FiniteWord_createFromAPInt(Size, Val);
}

FiniteWord *FiniteWord_createFromDecimalString(size_t Size, const char *str) {
    //llvm::outs() << "creating FiniteWordImpl: " << str << "\n";
    llvm::StringRef text(str);
    
    assert(Size != 0);
    
    // ignore size for now, will have to use size if we care about zero-padding of the string
    auto bits = FiniteWord_getBitsNeeded(str, 10);
    auto Val = llvm::APInt(bits, text, 10);
    return FiniteWord_createFromAPInt(bits, Val);
}

FiniteWord *FiniteWord_createFromVal(size_t numBits, uint64_t val) {
    auto Val = llvm::APInt(numBits, val, false);
    return FiniteWord_createFromAPInt(numBits, Val);
}

FiniteWord *FiniteWord_createFromFiniteWord(FiniteWord *src) {
    if (src->Size == 0) {
        return FiniteWord_createEmpty();
    }
    return FiniteWord_createFromAPInt(src->Size, src->Val);
}

// void FiniteWord_release(FiniteWordImpl *word) {
//     delete word;
// }

int32_t FiniteWord_newString(FiniteWord *word, char **str) {
    
    std::string resStr = std::string("`");
    
    auto valStr = FiniteWord_bits(word);
    resStr = resStr + valStr;
    
    resStr = resStr + "`";
    
    *str = strdup(resStr.c_str());
    
    return 0;
}

int32_t FiniteWord_newDecimalString(FiniteWord *word, char **str) {
    
    std::string resStr = std::string("");
    
    if (word->Size == 0) {
        resStr = "0";
    } else {
        resStr = word->Val.toString(10, false).c_str();
    }
    
    *str = strdup(resStr.c_str());
    
    return 0;
}

int32_t FiniteWord_newSignedDecimalString(FiniteWord *word, char **str) {
    
    std::string resStr = std::string("");
    
    if (word->Size == 0) {
        resStr = resStr + "0";
    }
    else {
        resStr = word->Val.toString(10, true).c_str();
    }
    
    *str = strdup(resStr.c_str());
    
    return 0;
}



size_t FiniteWord_size(FiniteWord *word) {
    return word->Size;
}


const std::string FiniteWord_bits(FiniteWord *word) {
    std::stringstream ss;
    for (decltype(word->Size) i = 0; i < word->Size; i++) {
        auto index = word->Size - 1 - i;
        auto bit = FiniteWord_getBit(word, static_cast<unsigned int>(index));
        if (bit == 1) {
            ss << '1';
        } else {
            ss << '0';
        }
    }
    return ss.str();
}

//void FiniteWord_releaseString(char *str) {
//    llvm::outs() << "deleting FiniteWordImpl string\n";
//    delete str;
//}






bool FiniteWord_equal(FiniteWord *A, FiniteWord *B) {
    return A->Val == B->Val;
}

bool FiniteWord_notEqual(FiniteWord *A, FiniteWord *B) {
    return A->Val != B->Val;
}

size_t FiniteWord_getBitWidth(FiniteWord *word) {
    return word->Val.getBitWidth();
}

void FiniteWord_setAllBits(FiniteWord *word) {
    word->Val.setAllBits();
}

void FiniteWord_clearAllBits(FiniteWord *word) {
    word->Val.clearAllBits();
}

//FiniteWordImpl& FiniteWordImpl_operator<<=(unsigned ShiftAmt) {
//    Val<<=ShiftAmt;
//    return *this;
//}
FiniteWord *FiniteWord_leftShift(FiniteWord *word, size_t ShiftAmount) {
    auto Shifted = word->Val << ShiftAmount;
    return FiniteWord_createFromAPInt(word->Size, Shifted);
}

//
//FiniteWordImpl& FiniteWordImpl_operator|=(const FiniteWordImpl &RHS) {
//    Val |= RHS.Val;
//    return *this;
//}

//bool FiniteWordImpl_operator[](unsigned bitPosition) const {
//    return Val[bitPosition];
//}
uint8_t FiniteWord_getBit(FiniteWord *word, size_t bitPosition) {
    return word->Val[bitPosition];
}

//char *toString(FiniteWordImpl *word, uint8_t Radix, bool Signed) {
//    return word->Val.toString(Radix, Signed).c_str();
//}

//bool FiniteWordImpl_operator==(const FiniteWordImpl &RHS) const {
//    return Val == RHS.Val;
//}
//
//bool FiniteWordImpl_operator!=(const FiniteWordImpl &RHS) const {
//    return Val != RHS.Val;
//}

FiniteWord *FiniteWord_rotateRight(FiniteWord *word, size_t rotateAmt) {
    auto Rotated = word->Val.rotr(rotateAmt);
    return FiniteWord_createFromAPInt(word->Size, Rotated);
}

FiniteWord *FiniteWord_rotateLeft(FiniteWord *word, size_t rotateAmt) {
    auto Rotated = word->Val.rotl(rotateAmt);
    return FiniteWord_createFromAPInt(word->Size, Rotated);
}

FiniteWord *FiniteWord_reverse(FiniteWord *word) {
    auto Reversed = word->Val.reverseBits();
    return FiniteWord_createFromAPInt(word->Size, Reversed);
}

//FiniteWordImpl FiniteWordImpl_operator~() const {
//    auto Not = ~Val;
//    return FiniteWordImpl(Not);
//}
FiniteWord *FiniteWord_not(FiniteWord *word) {
    return FiniteWord_createFromAPInt(word->Size, ~word->Val);
}

//FiniteWordImpl FiniteWordImpl_operator^(const FiniteWordImpl &b) const {
//    auto Xor = Val ^ b.Val;
//    return FiniteWordImpl(Xor);
//}
FiniteWord *FiniteWord_xor(FiniteWord *word, FiniteWord *RHS) {
    return FiniteWord_createFromAPInt(word->Size, word->Val ^ RHS->Val);
}

//
//FiniteWordImpl FiniteWordImpl_operator&(const FiniteWordImpl &b) const {
//    auto And = Val & b.Val;
//    return FiniteWordImpl(And);
//}
FiniteWord *FiniteWord_and(FiniteWord *word, FiniteWord *RHS) {
    return FiniteWord_createFromAPInt(word->Size, word->Val & RHS->Val);
}


//
//FiniteWordImpl FiniteWordImpl_operator|(const FiniteWordImpl &b) const {
//    auto Or = Val | b.Val;
//    return FiniteWordImpl(Or);
//}
FiniteWord *FiniteWord_or(FiniteWord *word, FiniteWord *RHS) {
    return FiniteWord_createFromAPInt(word->Size, word->Val | RHS->Val);
}


uint64_t FiniteWord_getRawData(FiniteWord *word) {
    return *word->Val.getRawData();
}

bool FiniteWord_ugt(FiniteWord *word, FiniteWord *RHS) {
    return word->Val.ugt(RHS->Val);
}

FiniteWord *FiniteWord_trunc(FiniteWord *word, size_t truncAmt) {
    auto Truncated = word->Val.trunc(truncAmt);
    return FiniteWord_createFromAPInt(truncAmt, Truncated);
}

FiniteWord *FiniteWord_residue(FiniteWord *word, size_t width) {
    assert(width <= word->Size && "Index too large");
    if (width == word->Size) {
        return word;
    }
    if (width == 0) {
        return FiniteWord_EMPTY;
    }
    return FiniteWord_trunc(word, width);
}

FiniteWord *FiniteWord_lshr(FiniteWord *word, size_t shiftAmt) {
    auto Shifted = word->Val.lshr(shiftAmt);
    return FiniteWord_createFromAPInt(word->Size, Shifted);
}

FiniteWord *FiniteWord_zext(FiniteWord *word, size_t width) {
    auto Extended = word->Val.zext(width);
    return FiniteWord_createFromAPInt(width, Extended);
}

bool FiniteWord_isSplat(FiniteWord *word, size_t SplatSizeInBits) {
    return word->Val.isSplat(SplatSizeInBits);
}

//FiniteWordImpl FiniteWordImpl_operator-() const {
//    auto Neg = -Val;
//    return FiniteWordImpl(Neg);
//}

//FiniteWordImpl FiniteWordImpl_operator-(const FiniteWordImpl &b) const {
//    auto Sub = Val - b.Val;
//    return FiniteWordImpl(Sub);
//}
//
//FiniteWordImpl FiniteWordImpl::operator+(const FiniteWordImpl &b) const {
//    auto Plus = Val + b.Val;
//    return FiniteWordImpl(Plus);
//}
//
//FiniteWordImpl FiniteWordImpl::operator*(const FiniteWordImpl &b) const {
//    auto Times = Val * b.Val;
//    return FiniteWordImpl(Times);
//}

FiniteWord *FiniteWord_minus(FiniteWord *word) {
    return FiniteWord_createFromAPInt(word->Size, -word->Val);
}

//    FiniteWordImpl operator-(const FiniteWordImpl &b);
FiniteWord *FiniteWord_subtract(FiniteWord *word, FiniteWord *RHS) {
    return FiniteWord_createFromAPInt(word->Size, word->Val - RHS->Val);
}

//    FiniteWordImpl operator+(const FiniteWordImpl &b);
FiniteWord *FiniteWord_add(FiniteWord *word, FiniteWord *RHS) {
    return FiniteWord_createFromAPInt(word->Size, word->Val + RHS->Val);
}

//    FiniteWordImpl operator*(const FiniteWordImpl &b);
FiniteWord *FiniteWord_multiply(FiniteWord *word, FiniteWord *RHS) {
    return FiniteWord_createFromAPInt(word->Size, word->Val * RHS->Val);
}

FiniteWord *FiniteWord_udiv(FiniteWord *word, FiniteWord *RHS) {
    auto Div = word->Val.udiv(RHS->Val);
    return FiniteWord_createFromAPInt(word->Size, Div);
}

FiniteWord *FiniteWord_urem(FiniteWord *word, FiniteWord *RHS) {
    auto Rem = word->Val.urem(RHS->Val);
    return FiniteWord_createFromAPInt(word->Size, Rem);
}

FiniteWord *FiniteWord_zextOrSelf(FiniteWord *word, size_t width) {
    auto Extended = word->Val.zextOrSelf(width);
    return FiniteWord_createFromAPInt(width, Extended);
}

size_t FiniteWord_countPopulation(FiniteWord *word) {
    return word->Val.countPopulation();
}














unsigned FiniteWord_getBitsNeeded(const char * str, uint8_t radix) {
    llvm::StringRef text(str);
    return llvm::APInt::getBitsNeeded(text, radix);
}

//FiniteWordImpl *FiniteWord_GreatestCommonDivisor(FiniteWordImpl *A, FiniteWordImpl *B) {
//    auto GCD = llvm::APIntOps::GreatestCommonDivisor(A->Val, B->Val);
//    return FiniteWord_createFromAPInt(A->Size, GCD);
//}

void FiniteWord_udivrem(FiniteWord *LHS, FiniteWord *RHS, FiniteWord **Quotient, FiniteWord **Remainder) {
    auto Size = FiniteWord_size(LHS);
    llvm::APInt TmpRemainder;
    llvm::APInt TmpQuotient;
    llvm::APInt::udivrem(LHS->Val, RHS->Val, TmpQuotient, TmpRemainder);
    *Remainder = FiniteWord_createFromAPInt(Size, TmpRemainder);
    *Quotient = FiniteWord_createFromAPInt(Size, TmpQuotient);
}





// namespace tuppence {
    
//     namespace finiteword {
        
// //        const FiniteWord EMPTY = FiniteWord::FactoryEmpty();
// //        const FiniteWord ZERO_1BIT = FiniteWord::FactoryBool((bool)0);
// //        const FiniteWord ONE_1BIT = FiniteWord::FactoryBool((bool)1);
        
//     }
    
// }

// using namespace tuppence;

/// Default constructor
//FiniteWord::FiniteWord() :
//Value(VK_FiniteWord), _size(0), Val() {}

//FiniteWord::FiniteWord(FiniteWordImpl *FiniteWordImplVal) :
//Value(VK_FiniteWord), _size(FiniteWordImpl_getBitWidth(FiniteWordImplVal)), Val(FiniteWordImplVal) {}

//const FiniteWord FiniteWord::FactoryEmpty() {
//    return FiniteWord();
//}

//const FiniteWord FiniteWord::FactoryString(std::string BinaryString) {
//    auto size = BinaryString.size();
//    if (size == 0) {
//        return finiteword::EMPTY;
//    }
//
//    auto Val = FiniteWordImpl_createFromString(size, BinaryString, 2);
//    return FactoryFiniteWordImpl(Val);
//}

//const FiniteWord FiniteWord::FactoryDecimalString(std::string DecimalString) {
//    assert(!DecimalString.empty() && "string is empty");
//    assert((DecimalString.size() == 1 || DecimalString[0] != '0') && "string is padded with 0");
//    auto needed = FiniteWordImpl_getBitsNeeded(DecimalString.c_str(), 10);
//    auto Val = FiniteWordImpl_createFromString(needed, DecimalString, 10);
//    return FactoryFiniteWordImpl(Val);
//}

FiniteWord *FiniteWord_createFromRepsWord(size_t Repetitions, FiniteWord *Pattern) {
    auto size = Repetitions * Pattern->Size;
    if (size == 0) {
        return FiniteWord_EMPTY;
    }

    // optimizations for all 0s and all 1s
    {
        auto pop = FiniteWord_countPopulation(Pattern);
        if (pop == 0) {
            // all 0s
            auto Accumulate = FiniteWord_createFromVal(static_cast<unsigned int>(size), (uint64_t)0);
            FiniteWord_clearAllBits(Accumulate);
            return Accumulate;
        }
        if (pop == Pattern->Size) {
            // all 1s
            auto Accumulate = FiniteWord_createFromVal(static_cast<unsigned int>(size), (uint64_t)0);
            FiniteWord_setAllBits(Accumulate);
            return Accumulate;
        }
    }

    auto Width = Math_bitLength(Repetitions);
    auto FiniteWordReps = llvm::APInt(static_cast<unsigned int>(Width), Repetitions, false);
    auto ReversedReps = FiniteWordReps.reverseBits();
    assert(ReversedReps[0] == 1 && "ReversedReps[0] must be 1");
    ReversedReps = ReversedReps.lshr(1);
    auto PatternExtended = FiniteWord_zextOrSelf(Pattern, static_cast<unsigned int>(size));
    auto Accumulate = PatternExtended;
    auto AccumulatedSize = Pattern->Size;
    while (AccumulatedSize < size) {
        // double the reps
        auto Original = Accumulate;
        Accumulate = FiniteWord_leftShift(Accumulate, static_cast<unsigned int>(AccumulatedSize));
        Accumulate = FiniteWord_or(Accumulate, Original);
        AccumulatedSize *= 2;
        if (ReversedReps[0] == 1) {
            // add 1 rep
            Accumulate = FiniteWord_leftShift(Accumulate, static_cast<unsigned int>(Pattern->Size));
            Accumulate = FiniteWord_or(Accumulate, PatternExtended);
            AccumulatedSize += Pattern->Size;
        }
        ReversedReps = ReversedReps.lshr(1);
    }
    assert(AccumulatedSize == size && "Accumulated size is not expected size");
    assert(ReversedReps.getActiveBits() == 0 && "ReversedReps is not properly processed");
    return Accumulate;
}

//const FiniteWord FiniteWord::FactoryBool(bool Bit) {
//    auto Val = FiniteWordImpl_createFromVal(1, (uint64_t)Bit);
//    return FiniteWord(Val);
//}

//const FiniteWord FiniteWord::FactoryFiniteWordImpl(FiniteWordImpl *Value) {
//    return FiniteWord(Value);
//}

//const FiniteWord FiniteWord::FactorySizeT(size_t Val) {
//    auto A = FiniteWordImpl_createFromVal(64, Val);
//    return FactoryFiniteWordImpl(A);
//}

/// Copy constructor
//FiniteWord::FiniteWord(const FiniteWord& other) :
//Value(VK_FiniteWord), _size(other._size) {
//    if (_size > 0) {
//        Val = other.Val;
//    }
//}

/// Copy assignment operator
//FiniteWord& FiniteWord::operator=(const FiniteWord& other) {
//    _size = other._size;
//    if (_size > 0) {
//        Val = other.Val;
//    }
//    return *this;
//}

/// Move constructor
//FiniteWord::FiniteWord(FiniteWord&& other) :
//Value(VK_FiniteWord), _size(other._size) {
//    if (_size > 0) {
//        Val = std::move(other.Val);
//    }
//}

/// Move assignment operator
//FiniteWord& FiniteWord::operator=(FiniteWord&& other) {
//    _size = other._size;
//    if (_size > 0) {
//        Val = std::move(other.Val);
//    }
//    return *this;
//}

//size_t FiniteWord::size() const {
//    return _size;
//}

//const std::string FiniteWord::bits() const {
//    std::stringstream ss;
//    for (decltype(_size) i = 0; i < _size; i++) {
//        auto index = _size - 1 - i;
//        auto bit = FiniteWordImpl_getBit(Val, static_cast<unsigned int>(index));
//        ss << bit;
//    }
//    return ss.str();
//}

//const std::string FiniteWord::string() const {
//    std::stringstream ss;
//    ss << "`";
//    ss << bits();
//    ss << "`";
//    return ss.str();
//}

//const std::string FiniteWord::typesetString() const {
//    std::stringstream ss;
//    ss << "\\unicode{96}";
//    ss << bits();
//    ss << "\\unicode{96}";
//    return ss.str();
//}

//bool FiniteWord::get(size_t index) const {
//    assert(index < _size && "Index is too large");
//    return FiniteWordImpl_getBit(Val, static_cast<unsigned int>(index));
//}

//bool FiniteWord::operator==(const FiniteWord other) const {
//    assert(_size == other._size && "Sizes not equal");
//    if (_size == 0) {
//        return true;
//    }
//    return FiniteWordImpl_equal(Val, other.Val);
//}

//bool FiniteWord::operator!=(const FiniteWord other) const {
//    assert(_size == other._size && "Sizes not equal");
//    if (_size == 0) {
//        return false;
//    }
//    return FiniteWordImpl_unequal(Val, other.Val);
//}

//
// bitwise operations
//

//const FiniteWord FiniteWord::operator~() const {
//    if (_size == 0) {
//        return finiteword::EMPTY;
//    }
//    auto Not = FiniteWordImpl_not(Val);
//    return FactoryFiniteWordImpl(Not);
//}

//const FiniteWord FiniteWord::operator^(FiniteWord other) const {
//    assert(_size == other._size && "Sizes not equal");
//    if (_size == 0) {
//        return finiteword::EMPTY;
//    }
//    auto Xor = FiniteWordImpl_xor(Val, other.Val);
//    return FactoryFiniteWordImpl(Xor);
//}

//const FiniteWord FiniteWord::operator&(FiniteWord other) const {
//    assert(_size == other._size && "Sizes not equal");
//    if (_size == 0) {
//        return finiteword::EMPTY;
//    }
//    auto And = FiniteWordImpl_and(Val, other.Val);
//    return FactoryFiniteWordImpl(And);
//}

//const FiniteWord FiniteWord::operator|(FiniteWord other) const {
//    assert(_size == other._size && "Sizes not equal");
//    if (_size == 0) {
//        return finiteword::EMPTY;
//    }
//    auto Or = FiniteWordImpl_or(Val, other.Val);
//    return FactoryFiniteWordImpl(Or);
//}

//const uint64_t* FiniteWord::getRawData() const {
//    return FiniteWordImpl_getRawData(Val);
//}

//
// Structural Operations
//

bool FiniteWord_arrayEqual(FiniteWord **ToTest, size_t Count) {
   assert(Count > 1 && "Vals does not contain more than one element");
   std::vector<FiniteWord *> Vals(ToTest, ToTest + Count);
   auto Iter = Vals.begin();
   auto First = *Iter;
   Iter++;
   for (; Iter != Vals.end(); ++Iter) {
       // maintain intuitive order
       if (FiniteWord_notEqual(*Iter, First)) {
           return false;
       }
   }
   return true;
}

bool FiniteWord_arrayNotEqual(FiniteWord **ToTest, size_t Count) {
   assert(Count > 1 && "Vals does not contain more than one element");
   std::vector<FiniteWord *> Vals(ToTest, ToTest + Count);
   for (auto Iter = Vals.begin(); Iter != Vals.end(); ++Iter) {
       auto First = *Iter;
       auto Iter2 = Iter;
       Iter2++;
       for (; Iter2 != Vals.end(); Iter2++) {
           auto Second = *Iter2;
           // maintain intuitive order
           if (FiniteWord_equal(Second, First)) {
               return false;
           }
       }
   }
   return true;
}

//bool FiniteWord::ugt(FiniteWord other) const {
//    assert(_size == other._size && "Sizes not equal");
//    return FiniteWordImpl_ugt(Val, other.Val);
//}

FiniteWord *FiniteWord_shiftRight(FiniteWord *word, size_t i) {
    assert(i <= word->Size && "Index too large");
    if (i == 0) {
        return word;
    }
    if (i == word->Size) {
        return FiniteWord_EMPTY;
    }
    auto Shifted = FiniteWord_lshr(word, i);
    Shifted = FiniteWord_trunc(Shifted, word->Size - i);
    return Shifted;
}

// this could be passed in as Hi or Lo, so make sure to make
// a copy first
void FiniteWord_shiftRightResidue(FiniteWord *word, size_t i, FiniteWord **Hi, FiniteWord **Lo) {
    assert(i <= word->Size && "Index too large");
    auto Tmp = *word;
    *Hi = FiniteWord_shiftRight(&Tmp, i);
    *Lo = FiniteWord_residue(&Tmp, i);
}

FiniteWord *FiniteWord_concatenate(FiniteWord *word, FiniteWord *other) {
   if (FiniteWord_size(word) == 0) {
       return other;
   }
   if (FiniteWord_size(other) == 0) {
       return word;
   }
   auto Res = FiniteWord_zextOrSelf(word, FiniteWord_size(word) + FiniteWord_size(other));
   auto B = FiniteWord_zextOrSelf(other, FiniteWord_size(word) + FiniteWord_size(other));
   Res = FiniteWord_leftShift(Res, FiniteWord_size(other));
   Res = FiniteWord_or(Res, B);
   return Res;
}

/// copied from ScalarEvolution.cpp
FiniteWord *FiniteWord_gcd(FiniteWord *A, FiniteWord *B) {
    
    auto GCD = llvm::APIntOps::GreatestCommonDivisor(A->Val, B->Val);
    return FiniteWord_createFromAPInt(A->Size, GCD);
}

void FiniteWord_compressPeriod(FiniteWord **period) {
    auto periodSize = (*period)->Size;
    auto sq = (size_t)sqrt(periodSize);
    for (decltype(sq) testDivisor = 1; testDivisor <= sq; testDivisor++) {
        if (periodSize % testDivisor == 0) {

            // first use testDivisor
            if (periodSize > 1) {
                if (FiniteWord_isSplat(*period, testDivisor)) {
                    auto modWord = FiniteWord_residue(*period, testDivisor);
                    *period = modWord;

                    // recurse on compressedPeriod, but it is now smaller
                    FiniteWord_compressPeriod(period);
                    return;
                }
            }

            // then use use periodSize / testDivisor
            if (testDivisor > 1) {
                if (FiniteWord_isSplat(*period, static_cast<unsigned int>(periodSize / testDivisor))) {
                    auto modWord = FiniteWord_residue(*period, periodSize / testDivisor);
                    *period = modWord;

                    // recurse on compressedPeriod, but it is now smaller
                    FiniteWord_compressPeriod(period);
                    return;
                }
            }
        }
    }
}

//bool FiniteWord::isPeriodCompressed(const FiniteWord period) {
//    assert(period._size > 0 && "Period size is 0");
//
//    auto periodSize = period._size;
//    auto sq = (size_t)sqrt(periodSize);
//    for (decltype(sq) testDivisor = 1; testDivisor <= sq; testDivisor++) {
//        if (periodSize % testDivisor == 0) {
//
//            // first use testDivisor
//            if (periodSize > 1) {
//                if (FiniteWordImpl_isSplat(period.Val, testDivisor)) {
//                    return false;
//                }
//            }
//
//            // then use periodSize / testDivisor
//            if (testDivisor > 1) {
//                if (FiniteWordImpl_isSplat(period.Val, static_cast<unsigned int>(periodSize / testDivisor))) {
//                    return false;
//                }
//            }
//        }
//    }
//
//    return true;
//}

void FiniteWord_multiplyPeriod(FiniteWord **period, size_t factor) {
    *period = FiniteWord_createFromRepsWord(factor, *period);
}

//
// Bitwise operations
//

//const FiniteWord FiniteWord::bitwise_not(FiniteWord L) {
//    return ~L;
//}

FiniteWord *FiniteWord_arrayOr(FiniteWord **Values, size_t Count) {
   assert(Count > 1 && "Vals does not contain more than one element");
   std::vector<FiniteWord *> Vals(Values, Values + Count);
   auto Iter = Vals.begin();
   auto Val = FiniteWord_createFromFiniteWord(*Iter);
   Iter++;
   for (; Iter != Vals.end(); ++Iter) {
       // maintain intuitive order
       Val = FiniteWord_or(*Iter, Val);
   }
   return Val;
}

FiniteWord *FiniteWord_arrayAnd(FiniteWord **Values, size_t Count) {
   assert(Count > 1 && "Vals does not contain more than one element");
   std::vector<FiniteWord *> Vals(Values, Values + Count);
   auto Iter = Vals.begin();
   auto Val = FiniteWord_createFromFiniteWord(*Iter);
   Iter++;
   for (; Iter != Vals.end(); ++Iter) {
       // maintain intuitive order
       Val = FiniteWord_and(*Iter, Val);
   }
   return Val;
}

FiniteWord *FiniteWord_arrayXor(FiniteWord **Values, size_t Count) {
   assert(Count > 1 && "Vals does not contain more than one element");
   std::vector<FiniteWord *> Vals(Values, Values + Count);
   auto Iter = Vals.begin();
   auto Val = FiniteWord_createFromFiniteWord(*Iter);
   Iter++;
   for (; Iter != Vals.end(); ++Iter) {
       // maintain intuitive order
       Val = FiniteWord_xor(*Iter, Val);
   }
   return Val;
}

//
// Arithmetic Operations
//

//const FiniteWord FiniteWord::operator-() const {
//    assert(_size > 0 && "size is 0");
//    auto Not = FiniteWordImpl_minus(Val);
//    return FactoryFiniteWordImpl(Not);
//}

//const FiniteWord FiniteWord::operator+(FiniteWord other) const {
//    assert(_size > 0 && "size is 0");
//    assert(_size == other._size && "Sizes not equal");
//    auto Added = FiniteWordImpl_add(Val, other.Val);
//    return FactoryFiniteWordImpl(Added);
//}

//const FiniteWord FiniteWord::operator-(FiniteWord other) const {
//    assert(_size > 0 && "size is 0");
//    assert(_size == other._size && "Sizes not equal");
//    auto Minused = FiniteWordImpl_subtract(Val, other.Val);
//    return FactoryFiniteWordImpl(Minused);
//}

//const FiniteWord FiniteWord::operator*(FiniteWord other) const {
//    assert(_size > 0 && "size is 0");
//    assert(_size == other._size && "Sizes not equal");
//    auto Multiplied = FiniteWordImpl_multiply(Val, other.Val);
//    return FactoryFiniteWordImpl(Multiplied);
//}

//const FiniteWord FiniteWord::operator/(FiniteWord other) const {
//    assert(_size > 0 && "size is 0");
//    assert(_size == other._size && "Sizes not equal");
//    auto Divided = FiniteWordImpl_udiv(Val, other.Val);
//    return FactoryFiniteWordImpl(Divided);
//}

//const FiniteWord FiniteWord::operator%(FiniteWord other) const {
//    assert(_size > 0 && "size is 0");
//    assert(_size == other._size && "Sizes not equal");
//    auto Modded = FiniteWordImpl_urem(Val, other.Val);
//    return FactoryFiniteWordImpl(Modded);
//}

void FiniteWord_arrayPlus(FiniteWord **Values, size_t Count, FiniteWord **Hi, FiniteWord **Lo) {
   assert(Count > 1 && "Vals does not contain more than one element");
   std::vector<FiniteWord *> Vals(Values, Values + Count);
   auto Iter = Vals.begin();
   size_t OldWidth = FiniteWord_size(*Iter);
   if (OldWidth == 0) {
       // Still return 1 carry bit, even if the words are empty
       *Hi = FiniteWord_ZERO_1BIT;
       *Lo = FiniteWord_EMPTY;
       return;
   }

   auto CarryWidth = Math_bitLength(Vals.size() - 1);

   size_t NewWidth = OldWidth + CarryWidth;
   auto APVal = FiniteWord_zext(*Iter, static_cast<unsigned int>(NewWidth));
   Iter++;
    
   for (; Iter != Vals.end(); ++Iter) {
       assert(FiniteWord_size(*Iter) == OldWidth && "Sizes not equal");
       // maintain intuitive order
       auto NextVal = FiniteWord_zext(*Iter, static_cast<unsigned int>(NewWidth));
       APVal = FiniteWord_add(NextVal, APVal);
   }

   *Lo = FiniteWord_residue(APVal, static_cast<unsigned int>(OldWidth));
   *Hi = FiniteWord_lshr(APVal, static_cast<unsigned int>(OldWidth));
   *Hi = FiniteWord_residue(*Hi, static_cast<unsigned int>(CarryWidth));
}

void FiniteWord_arraySubtract(FiniteWord **Values, size_t Count, FiniteWord **Hi, FiniteWord **Lo) {
    assert(Count == 2);
    FiniteWord *R = Values[0];
    FiniteWord *L = Values[1];
    assert(L->Size == R->Size && "Sizes not equal");
    auto OldWidth = L->Size;
    if (OldWidth == 0) {
        // Still return 1 carry bit, even if the words are empty
        *Hi = FiniteWord_ZERO_1BIT;
        *Lo = FiniteWord_EMPTY;
        return;
    }

    auto BorrowWidth = 1;

    size_t NewWidth = OldWidth + BorrowWidth;
    auto NewLVal = FiniteWord_zext(L, NewWidth);
    auto NewRVal = FiniteWord_zext(R, NewWidth);

    auto NewVal = FiniteWord_subtract(NewLVal, NewRVal);

    auto HiAPInt = FiniteWord_trunc(FiniteWord_lshr(NewVal, OldWidth), BorrowWidth);
    *Hi = HiAPInt;
    auto LoAPInt = FiniteWord_trunc(NewVal, OldWidth);
    *Lo = LoAPInt;
}

void FiniteWord_arrayTimes(FiniteWord **Values, size_t Count, FiniteWord **Hi, FiniteWord **Lo) {
   assert(Count > 1 && "Vals does not contain more than one element");
   std::vector<FiniteWord *> Vals(Values, Values + Count);
   auto Iter = Vals.begin();
   size_t OldWidth = FiniteWord_size(*Iter);
   if (OldWidth == 0) {
       // Still return 1 Hi bit, even if the words are empty
       *Hi = FiniteWord_ZERO_1BIT;
       *Lo = FiniteWord_EMPTY;
       return;
   }

   size_t NewWidth = OldWidth * Vals.size();
   auto APVal = FiniteWord_zext(*Iter, static_cast<unsigned int>(NewWidth));
   Iter++;

   for (; Iter != Vals.end(); ++Iter) {
       assert(FiniteWord_size(*Iter) == OldWidth && "Sizes not equal");
       // maintain intuitive order
       APVal = FiniteWord_multiply(FiniteWord_zext(*Iter, static_cast<unsigned int>(NewWidth)), APVal);
   }
   *Hi = FiniteWord_residue(FiniteWord_lshr(APVal, static_cast<unsigned int>(OldWidth)), static_cast<unsigned int>(OldWidth * (Vals.size() - 1)));
   *Lo = FiniteWord_residue(APVal, static_cast<unsigned int>(OldWidth));
}

//void FiniteWord::divide(FiniteWord Dividend, FiniteWord Divisor, FiniteWord& Quotient, FiniteWord& Remainder) {
//    assert(Dividend._size == Divisor._size && "Sizes not equal");
//    auto OldWidth = Dividend._size;
//    if (OldWidth == 0) {
//        Remainder = finiteword::EMPTY;
//        Quotient = finiteword::EMPTY;
//        return;
//    }
//
//    FiniteWordImpl_udivrem(Dividend.Val, Divisor.Val, Quotient.Val, Remainder.Val);
//}

//
// GTest
//

//void tuppence::PrintTo(const FiniteWord& bar, ::std::ostream* os) {
//    *os << bar.string();
//}

