//===------ RationalWord.h - RationalWord class definition ----------------===//
//
// The Tuppence Programming Language
//
// This file is distributed under the MIT Open Source License.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

#ifndef TUPPENCE_RATIONALWORD_H
#define TUPPENCE_RATIONALWORD_H

#include "FiniteWord.h"
#include "Value.h"

#include <string>
#include <vector>

namespace tuppence {

	class RationalWord : public Value {
		FiniteWord period;
		FiniteWord transient;

		RationalWord(FiniteWord Period, FiniteWord Transient);

		void calculateFraction(RationalWord& Numerator, RationalWord& Denominator) const;

		//
		// modify period and transient before creating RationalWord
		//
		static void reduce(FiniteWord& period, FiniteWord& transient);

		static const bool isReduced(FiniteWord period, FiniteWord transient);

		// extend transient to be size extent
		static void extendTransient(FiniteWord& period, FiniteWord& transient, size_t extent);

		static void alignPeriodAndTransient(
			FiniteWord& periodL, FiniteWord& transientL,
			FiniteWord& periodR, FiniteWord& transientR);

		static void windupTransient(FiniteWord& period, FiniteWord& transient);


	public:
		static const RationalWord FactoryPeriodTransient(FiniteWord period, FiniteWord transient);
		static const RationalWord FactoryString(std::string);
		static const RationalWord FactorySizeT(size_t);

		/// Default constructor
		RationalWord();

		/// Copy constructor
		RationalWord(const RationalWord& other);

		/// Copy assignment operator
		RationalWord& operator=(const RationalWord& rhs);

		/// Move constructor
		RationalWord(RationalWord&& other);

		/// Move assignment operator
		RationalWord& operator= (RationalWord&& other);

		const FiniteWord getPeriod() const;
		const FiniteWord getTransient() const;

		const std::string string() const override;

		const std::string representation() const;

		const std::string decimal() const;

		const bool isNonNegativeInteger() const;

		const bool isNegativeInteger() const;

		const RationalWord getNumerator() const;

		const RationalWord getDenominator() const;

		const uint64_t getUInt64Value() const;

		//
		// structural operations
		//

		const bool operator==(RationalWord) const;
		const bool operator!=(RationalWord) const;

		static const FiniteWord equal(std::vector<RationalWord>);
		static const FiniteWord notequal(std::vector<RationalWord>);
		
		const FiniteWord residue(size_t i) const;
		const RationalWord shiftRight(size_t i) const;
		void shiftRightResidue(size_t i, RationalWord& Hi, FiniteWord& Lo) const;
		
		const RationalWord concatenate(FiniteWord) const;

		//
		// bitwise operations
		//
		
		const RationalWord operator~() const;
		const RationalWord operator|(RationalWord) const;
		const RationalWord operator&(RationalWord) const;
		const RationalWord operator^(RationalWord) const;

		static const RationalWord bitwise_not(RationalWord);
		static const RationalWord bitwise_or(std::vector<RationalWord>);
		static const RationalWord bitwise_and(std::vector<RationalWord>);
		static const RationalWord bitwise_xor(std::vector<RationalWord>);

		//
		// arithmetic operations
		//

		const RationalWord operator-() const;
		const RationalWord operator+(RationalWord) const;
		const RationalWord operator-(RationalWord) const;
		const RationalWord operator*(RationalWord) const;
		const RationalWord operator/(RationalWord) const;

		static const RationalWord plus(std::vector<RationalWord>);
		static const RationalWord minus(RationalWord, RationalWord);
		static const RationalWord times(std::vector<RationalWord>);
		static const RationalWord divide(RationalWord, RationalWord);

		//
		// LLVM RTTI
		//
		static bool classof(const Value *S) {
			return S->getKind() == VK_RationalWord;
		}
	};

	namespace rationalword {

		extern const RationalWord ZERO;
		extern const RationalWord ONE;
		extern const RationalWord MINUS_ONE;

	}

	//
	// GTest
	//
	void PrintTo(const RationalWord& bar, ::std::ostream* os);
}

#endif
