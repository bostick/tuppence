//===------ FiniteWord.h - FiniteWord class definition --------------------===//
//
// The Tuppence Programming Language
//
// This file is distributed under the MIT Open Source License.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

#pragma once

//#include "finiteword/FiniteWordImpl.h"
// #include "../common/FiniteWordImpl.h"
#include "TuppenceValue.h"

// #include "common/TuppenceValue.h"

#include <memory>
#include <string>

#include <cstddef> // for size_t


#ifdef _WIN32
#    if RUNTIME_DLL
#    define RUNTIME_API __declspec(dllexport)
#    else
#    define RUNTIME_API __declspec(dllimport)
#endif
#else
#define RUNTIME_API
#endif

extern "C" RUNTIME_API {

    void FiniteWord_initialize();

    FiniteWord *FiniteWord_createEmpty();

    FiniteWord *FiniteWord_createFromBool(bool Bit);

    FiniteWord *FiniteWord_createFromBinaryString(size_t numBits, const char *str);
    
    FiniteWord *FiniteWord_createFromDecimalString(size_t numBits, const char *str);
    
    
    FiniteWord *FiniteWord_createFromVal(size_t numBits, uint64_t val);
    
    FiniteWord *FiniteWord_createFromFiniteWord(FiniteWord *src);
    
    FiniteWord *FiniteWord_createFromRepsWord(size_t RepetitionCount, FiniteWord *Pattern);
    
    
    
    
    
    
    
//    void FiniteWord_release(FiniteWordImpl *word);
    
    int32_t FiniteWord_newString(FiniteWord *word, char **str);
    
    int32_t FiniteWord_newDecimalString(FiniteWord *word, char **str);
    int32_t FiniteWord_newSignedDecimalString(FiniteWord *word, char **str);
    
//    void FiniteWord_releaseString(char *str);
    
    //int64_t FiniteWord_sleep(int64_t, int64_t);
    
    
    /// The number of bits in the FiniteWord
    size_t FiniteWord_size(FiniteWord *word);
    
    /// The width of the FiniteWord (the position of the last set bit)
    size_t FiniteWord_getBitWidth(FiniteWord *word);
    
    size_t FiniteWord_countPopulation(FiniteWord *word);
    
    
    void FiniteWord_clearAllBits(FiniteWord *word);
    
    void FiniteWord_setAllBits(FiniteWord *word);
    
    
    bool FiniteWord_equal(FiniteWord *A, FiniteWord *B);
    bool FiniteWord_notEqual(FiniteWord *A, FiniteWord *B);
    
    FiniteWord *FiniteWord_residue(FiniteWord *word, size_t i);
    
    
    
    
    void FiniteWord_multiplyPeriod(FiniteWord **period, size_t factor);
    void FiniteWord_compressPeriod(FiniteWord **period);
    
    
    
    
    
    
    
    
    FiniteWord *FiniteWord_zextOrSelf(FiniteWord *word, size_t width);
    
//    FiniteWordImpl& operator<<=(unsigned ShiftAmt);
    FiniteWord *FiniteWord_leftShift(FiniteWord *word, size_t ShiftAmt);
    
//    FiniteWordImpl& operator|=(const FiniteWordImpl &RHS);
//    FiniteWordImpl *FiniteWord_bitwiseOr(FiniteWordImpl *word, FiniteWordImpl *RHS);
    
//    bool operator[](unsigned bitPosition);
    uint8_t FiniteWord_getBit(FiniteWord *word, size_t bitPosition);
    
    FiniteWord *FiniteWord_rotateRight(FiniteWord *w, size_t amount);
    FiniteWord *FiniteWord_rotateLeft(FiniteWord *w, size_t amount);
    FiniteWord *FiniteWord_reverse(FiniteWord *w);
    
    
    
    const char *FiniteWord_toString(FiniteWord *word, uint8_t Radix, bool Signed);
    
//    bool operator==(const FiniteWordImpl &RHS);
    
//    bool operator!=(const FiniteWordImpl &RHS);
    
//    FiniteWordImpl operator~();
    FiniteWord *FiniteWord_not(FiniteWord *word);
    
//    FiniteWordImpl operator^(const FiniteWordImpl &b);
    FiniteWord *FiniteWord_xor(FiniteWord *word, FiniteWord *other);
    
//    FiniteWordImpl operator&(const FiniteWordImpl &b);
    FiniteWord *FiniteWord_and(FiniteWord *word, FiniteWord *other);
    
//    FiniteWordImpl operator|(const FiniteWordImpl &b);
    FiniteWord *FiniteWord_or(FiniteWord *word, FiniteWord *other);
    
    uint64_t FiniteWord_getRawData(FiniteWord *word);
    
    bool FiniteWord_ugt(FiniteWord *word, FiniteWord *RHS);
    
    FiniteWord *FiniteWord_lshr(FiniteWord *word, size_t shiftAmt);
    
    FiniteWord *FiniteWord_zext(FiniteWord *word, size_t width);
    
    bool FiniteWord_isSplat(FiniteWord *word, size_t SplatSizeInBits);
    
//    FiniteWordImpl operator-();
    FiniteWord *FiniteWord_minus(FiniteWord *word);
    
//    FiniteWordImpl operator-(const FiniteWordImpl &b);
    FiniteWord *FiniteWord_subtract(FiniteWord *word, FiniteWord *RHS);
    
//    FiniteWordImpl operator+(const FiniteWordImpl &b);
    FiniteWord *FiniteWord_add(FiniteWord *word, FiniteWord *RHS);
    
//    FiniteWordImpl operator*(const FiniteWordImpl &b);
    FiniteWord *FiniteWord_multiply(FiniteWord *word, FiniteWord *RHS);
    
    FiniteWord *FiniteWord_udiv(FiniteWord *word, FiniteWord *RHS);
    
    FiniteWord *FiniteWord_urem(FiniteWord *word, FiniteWord *RHS);
    
    
    
    
    
    
    
    unsigned FiniteWord_getBitsNeeded(const char * str, uint8_t radix);
    
//    FiniteWordImpl *FiniteWord_GreatestCommonDivisor(FiniteWordImpl *A, FiniteWordImpl *B);
    
    void FiniteWord_udivrem(FiniteWord *LHS, FiniteWord *RHS, FiniteWord **Quotient, FiniteWord **Remainder);







	bool FiniteWord_arrayEqual(FiniteWord **ToTest, size_t Count);
    bool FiniteWord_arrayNotEqual(FiniteWord **ToTest, size_t Count);

    FiniteWord *FiniteWord_arrayOr(FiniteWord **Values, size_t Count);
    FiniteWord *FiniteWord_arrayAnd(FiniteWord **Values, size_t Count);
    FiniteWord *FiniteWord_arrayXor(FiniteWord **Values, size_t Count);

    void FiniteWord_arrayPlus(FiniteWord **Values, size_t Count, FiniteWord **Carry, FiniteWord **Result);
    void FiniteWord_arraySubtract(FiniteWord **Values, size_t Count, FiniteWord **Hi, FiniteWord **Lo);
    void FiniteWord_arrayTimes(FiniteWord **Values, size_t Count, FiniteWord **Hi, FiniteWord **Lo);
    
    FiniteWord *FiniteWord_shiftRight(FiniteWord *word, size_t i);
    void FiniteWord_shiftRightResidue(FiniteWord *word, size_t i, FiniteWord **Hi, FiniteWord **Lo);



    FiniteWord *FiniteWord_concatenate(FiniteWord *word, FiniteWord *other);

    
    
    FiniteWord *FiniteWord_gcd(FiniteWord *a, FiniteWord *b);

        
    extern FiniteWord *FiniteWord_EMPTY;
    extern FiniteWord *FiniteWord_ZERO_1BIT;
    extern FiniteWord *FiniteWord_ONE_1BIT;
        

}


// namespace tuppence {
    
//    class FiniteWord : public Value {
//        size_t _size;
//        FiniteWordImpl *Val;
//
//        FiniteWord(FiniteWordImpl *);
//        static const FiniteWord FactoryFiniteWordImpl(FiniteWordImpl *);
//
//    public:
//        static const FiniteWord FactoryEmpty();
//        static const FiniteWord FactoryString(std::string BinaryString);
//        static const FiniteWord FactoryDecimalString(std::string DecimalString);
//
//        static const FiniteWord FactoryBool(bool Bit);
//        static const FiniteWord FactorySizeT(size_t Val);
//
//        /// Default constructor
//        FiniteWord();
//
//        /// Copy constructor
//        FiniteWord(const FiniteWord& other);
//
//        /// Copy assignment operator
//        FiniteWord& operator=(const FiniteWord& other);
//
//        /// Move constructor
//        FiniteWord(FiniteWord&& other);
//
//        /// Move assignment operator
//        FiniteWord& operator=(FiniteWord&& other);
//
//        size_t size() const;
//
//        virtual const std::string string() const override;
//
//        virtual const std::string typesetString() const override;
//
//        const std::string bits() const;
//
//        bool get(size_t index) const;
//
//        const FiniteWord rotateRight(size_t amount) const;
//        const FiniteWord rotateLeft(size_t amount) const;
//        const FiniteWord reverse() const;
//
//        const uint64_t* getRawData() const;
//
//        static const FiniteWord gcd(FiniteWord a, FiniteWord b);
//
//        //
//        // structural operations
//        //
//
//        bool operator==(FiniteWord other) const;
//        bool operator!=(FiniteWord other) const;
//
//        /// Unsigned Greater Than
//        bool ugt(FiniteWord other) const;
//
        // static const FiniteWordImpl equal(std::vector<FiniteWordImpl> ToTest);
        // static const FiniteWordImpl notequal(std::vector<FiniteWordImpl> ToTest);
//
//        const FiniteWord residue(size_t i) const;
//        const FiniteWord shiftRight(size_t i) const;
//        void shiftRightResidue(size_t i, FiniteWord& Hi, FiniteWord& Lo) const;
//        const FiniteWord concatenate(FiniteWord other) const;
//
//        static void compressPeriod(FiniteWord& period);
//        static void multiplyPeriod(FiniteWord& period, size_t factor);
//        static bool isPeriodCompressed(FiniteWord period);
//
//        //
//        // bitwise operations
//        //
//
//        const FiniteWord operator~() const;
//        const FiniteWord operator^(FiniteWord other) const;
//        const FiniteWord operator&(FiniteWord other) const;
//        const FiniteWord operator|(FiniteWord other) const;
//
//        static const FiniteWord bitwise_not(FiniteWord other);
//        static const FiniteWord bitwise_or(std::vector<FiniteWord>);
//        static const FiniteWord bitwise_and(std::vector<FiniteWord>);
//        static const FiniteWord bitwise_xor(std::vector<FiniteWord>);
//
//        //
//        // arithmetic operations
//        //
//
//        const FiniteWord operator-() const;
//        const FiniteWord operator+(FiniteWord other) const;
//        const FiniteWord operator-(FiniteWord other) const;
//        const FiniteWord operator*(FiniteWord other) const;
//        const FiniteWord operator/(FiniteWord other) const;
//        const FiniteWord operator%(FiniteWord other) const;
//
//        static void plus(std::vector<FiniteWord> Values, FiniteWord& Carry, FiniteWord& Result);
//        static void minus(FiniteWord Minuend, FiniteWord Subtrahend, FiniteWord& Borrow, FiniteWord& Result);
//        static void times(std::vector<FiniteWord> Values, FiniteWord& Hi, FiniteWord& Lo);
//        static void divide(FiniteWord Dividend, FiniteWord Divisor, FiniteWord& Quotient, FiniteWord& Remainder);
//
//        //
//        // LLVM RTTI
//        //
//        static bool classof(const Value *S) {
//            return S->getKind() == VK_FiniteWord;
//        }
//    };
    

    
    //
    // GTest
    //
//    void PrintTo(const FiniteWord& bar, ::std::ostream* os);
    
// } // end namespace tuppence
