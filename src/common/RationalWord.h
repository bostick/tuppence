//===------ RationalWord.h - RationalWord class definition ----------------===//
//
// The Tuppence Programming Language
//
// This file is distributed under the MIT Open Source License.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

#pragma once

#include "TuppenceValue.h"

#include <vector>


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

    void RationalWord_initialize();

    RationalWord *RationalWord_createFromPeriodTransient(FiniteWord *period, FiniteWord *transient);
    
    RationalWord *RationalWord_createFromRationalWord(RationalWord *);
    
    RationalWord *RationalWord_createFromDecimalString(const char *StrVal);
    
    RationalWord *RationalWord_createFromVal(size_t numBits, uint64_t val, bool nonNegative);


    int32_t RationalWord_newString(RationalWord *word, char **str);
    
    
    FiniteWord *RationalWord_period(RationalWord *rat);
    FiniteWord *RationalWord_transient(RationalWord *rat);
    
    RationalWord *RationalWord_numerator(RationalWord *rat);
    RationalWord *RationalWord_denominator(RationalWord *rat);

    
    bool RationalWord_equal(RationalWord *A, RationalWord *B);
    bool RationalWord_notEqual(RationalWord *A, RationalWord *B);
    
	bool RationalWord_arrayEqual(RationalWord **ToTest, size_t Count);
    bool RationalWord_arrayNotEqual(RationalWord **ToTest, size_t Count);
    
    
    bool RationalWord_isNegativeInteger(RationalWord *word);
    bool RationalWord_isNonNegativeInteger(RationalWord *word);
    uint64_t RationalWord_integerValue(RationalWord* word);
    
    
    
    
    
    FiniteWord *RationalWord_residue(RationalWord *rat, size_t i);
    RationalWord *RationalWord_shiftRight(RationalWord *word, size_t i);
    void RationalWord_shiftRightResidue(RationalWord *word, size_t i, RationalWord **Hi, FiniteWord **Lo);
    
    RationalWord *RationalWord_or(RationalWord *A, RationalWord *B);
    RationalWord *RationalWord_and(RationalWord *A, RationalWord *B);
    RationalWord *RationalWord_xor(RationalWord *A, RationalWord *B);
    RationalWord *RationalWord_not(RationalWord *A);
    
    
    RationalWord *RationalWord_arrayOr(RationalWord **Values, size_t Count);
    RationalWord *RationalWord_arrayAnd(RationalWord **Values, size_t Count);
    RationalWord *RationalWord_arrayXor(RationalWord **Values, size_t Count);
    
    RationalWord *RationalWord_plus(RationalWord *, RationalWord*);
    RationalWord *RationalWord_subtract(RationalWord *, RationalWord*);
    RationalWord *RationalWord_times(RationalWord *, RationalWord*);
    RationalWord *RationalWord_divide(RationalWord *, RationalWord*);
    void RationalWord_quotientRemainder(RationalWord *L, RationalWord *R, RationalWord **Quotient, RationalWord **Remainder);
    RationalWord *RationalWord_minus(RationalWord *);
    
    RationalWord *RationalWord_arrayPlus(RationalWord **Values, size_t Count);
    RationalWord *RationalWord_arrayTimes(RationalWord **Values, size_t Count);



    RationalWord *RationalWord_concatenate(RationalWord *, FiniteWord *);
    
    
    
    extern RationalWord *RationalWord_ONE;
    extern RationalWord *RationalWord_ZERO;
    extern RationalWord *RationalWord_MINUS_ONE;
    
}

// namespace tuppence {
    
//    class RationalWord : public Value {
//        FiniteWord period;
//        FiniteWord transient;
//
//        RationalWord(FiniteWord Period, FiniteWord Transient);
//
//        void calculateFraction(RationalWord& Numerator, RationalWord& Denominator) const;
//
//        //
//        // modify period and transient before creating RationalWord
//        //
//        static void reduce(FiniteWord& period, FiniteWord& transient);
//
//        static bool isReduced(FiniteWord period, FiniteWord transient);
//
//        // extend transient to be size extent
//        static void extendTransient(FiniteWord& period, FiniteWord& transient, size_t extent);
//
//        static void alignPeriodAndTransient(
//                                            FiniteWord& periodL, FiniteWord& transientL,
//                                            FiniteWord& periodR, FiniteWord& transientR);
//
//        static void windupTransient(FiniteWord& period, FiniteWord& transient);
//
//
//    public:
//        static const RationalWord FactoryPeriodTransient(FiniteWord period, FiniteWord transient);
//        static const RationalWord FactoryString(std::string);
//        static const RationalWord FactorySizeT(size_t);
//
//        /// Default constructor
//        RationalWord();
//
//        /// Copy constructor
//        RationalWord(const RationalWord& other);
//
//        /// Copy assignment operator
//        RationalWord& operator=(const RationalWord& rhs);
//
//        /// Move constructor
//        RationalWord(RationalWord&& other);
//
//        /// Move assignment operator
//        RationalWord& operator= (RationalWord&& other);
//
//        const FiniteWord getPeriod() const;
//        const FiniteWord getTransient() const;
//
//        const std::string string() const override;
//
//        const std::string typesetString() const override;
//
//        const std::string decimal() const;
//
//        bool isNonNegativeInteger() const;
//
//        bool isNegativeInteger() const;
//
//        const RationalWord getNumerator() const;
//
//        const RationalWord getDenominator() const;
//
//        uint64_t getUInt64Value() const;
//
//        //
//        // structural operations
//        //
//
//        bool operator==(RationalWord) const;
//        bool operator!=(RationalWord) const;
//
//        static const FiniteWord equal(std::vector<RationalWord>);
//        static const FiniteWord notequal(std::vector<RationalWord>);
//
//        const FiniteWord residue(size_t i) const;
//        const RationalWord shiftRight(size_t i) const;
//        void shiftRightResidue(size_t i, RationalWord& Hi, FiniteWord& Lo) const;
//
//        
//
//        //
//        // bitwise operations
//        //
//
//        const RationalWord operator~() const;
//        const RationalWord operator|(RationalWord) const;
//        const RationalWord operator&(RationalWord) const;
//        const RationalWord operator^(RationalWord) const;
//
//        static const RationalWord bitwise_not(RationalWord);
//        static const RationalWord bitwise_or(std::vector<RationalWord>);
//        static const RationalWord bitwise_and(std::vector<RationalWord>);
//        static const RationalWord bitwise_xor(std::vector<RationalWord>);
//
//        //
//        // arithmetic operations
//        //
//
//        const RationalWord operator-() const;
//        const RationalWord operator+(RationalWord) const;
//        const RationalWord operator-(RationalWord) const;
//        const RationalWord operator*(RationalWord) const;
//        const RationalWord operator/(RationalWord) const;
//
//        static const RationalWord plus(std::vector<RationalWord>);
//        static const RationalWord minus(RationalWord, RationalWord);
//        static const RationalWord times(std::vector<RationalWord>);
//        static const RationalWord divide(RationalWord, RationalWord);
//
//        //        static void integerDivide(RationalWord Dividend, uint64_t Divisor, RationalWord& Quotient, RationalWord& Remainder);
//
//        //
//        // LLVM RTTI
//        //
//        static bool classof(const Value *S) {
//            return S->getKind() == VK_RationalWord;
//        }
//    };
    
    // namespace rationalword {
        
//        extern const RationalWord ZERO;
//        extern const RationalWord ONE;
//        extern const RationalWord MINUS_ONE;
        
    // }
    
    //
    // GTest
    //
//    void PrintTo(const RationalWord& bar, ::std::ostream* os);
    
// } // end namespace tuppence
