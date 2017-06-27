//===------ FiniteWord.h - FiniteWord class definition --------------------===//
//
// The Tuppence Programming Language
//
// This file is distributed under the MIT Open Source License.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

#pragma once

#include "Value.h"

#include <memory>
#include <string>

#include <cstddef> // for size_t

// Forward declare APInt (and use pointers to APInt) to prevent any dependencies on APInt creeping out of FiniteWord
namespace llvm {
	class APInt;
}

namespace tuppence {

	class FiniteWord : public Value {
		size_t size;
		std::shared_ptr<llvm::APInt> Val;

		FiniteWord(std::shared_ptr<llvm::APInt>);
		static const FiniteWord FactoryAPInt(std::shared_ptr<llvm::APInt>);

	public:
		static const FiniteWord FactoryEmpty();
		static const FiniteWord FactoryString(std::string BinaryString);
		static const FiniteWord FactoryDecimalString(std::string DecimalString);
		static const FiniteWord FactoryRepsWord(size_t RepetitionCount, FiniteWord Pattern);
		static const FiniteWord FactoryBool(bool Bit);
		static const FiniteWord FactorySizeT(size_t Val);

		/// Default constructor
		FiniteWord();

		/// Copy constructor
		FiniteWord(const FiniteWord& other);

		/// Copy assignment operator
		FiniteWord& operator=(const FiniteWord& other);

		/// Move constructor
		FiniteWord(FiniteWord&& other);

		/// Move assignment operator
		FiniteWord& operator=(FiniteWord&& other);

		const size_t getSize() const;

		const std::string string() const override;

		const std::string bits() const;

		const std::string decimal() const;
		const std::string decimalSigned() const;

		const bool get(size_t index) const;

		const FiniteWord rotateRight(size_t amount) const;
		const FiniteWord rotateLeft(size_t amount) const;
		const FiniteWord reverse() const;

		const uint64_t* getRawData() const;

		static const FiniteWord gcd(FiniteWord a, FiniteWord b);

		//
		// structural operations
		//

		const bool operator==(FiniteWord other) const;
		const bool operator!=(FiniteWord other) const;
		
		/// Unsigned Greater Than
		const bool ugt(FiniteWord other) const;

		static const FiniteWord equal(std::vector<FiniteWord> ToTest);
		static const FiniteWord notequal(std::vector<FiniteWord> ToTest);
		
		const FiniteWord residue(size_t i) const;
		const FiniteWord shiftRight(size_t i) const;
		void shiftRightResidue(size_t i, FiniteWord& Hi, FiniteWord& Lo) const;
		const FiniteWord concatenate(FiniteWord other) const;

		static void compressPeriod(FiniteWord& period);
		static void multiplyPeriod(FiniteWord& period, size_t factor);
		static const bool isPeriodCompressed(FiniteWord period);

		//
		// bitwise operations
		//
		
		const FiniteWord operator~() const;
		const FiniteWord operator^(FiniteWord other) const;
		const FiniteWord operator&(FiniteWord other) const;
		const FiniteWord operator|(FiniteWord other) const;

		static const FiniteWord bitwise_not(FiniteWord other);
		static const FiniteWord bitwise_or(std::vector<FiniteWord>);
		static const FiniteWord bitwise_and(std::vector<FiniteWord>);
		static const FiniteWord bitwise_xor(std::vector<FiniteWord>);

		//
		// arithmetic operations
		//

		const FiniteWord operator-() const;
		const FiniteWord operator+(FiniteWord other) const;
		const FiniteWord operator-(FiniteWord other) const;
		const FiniteWord operator*(FiniteWord other) const;
		const FiniteWord operator/(FiniteWord other) const;
		const FiniteWord operator%(FiniteWord other) const;

		static void plus(std::vector<FiniteWord> Values, FiniteWord& Carry, FiniteWord& Result);
		static void minus(FiniteWord Minuend, FiniteWord Subtrahend, FiniteWord& Borrow, FiniteWord& Result);
		static void times(std::vector<FiniteWord> Values, FiniteWord& Hi, FiniteWord& Lo);
		static void divide(FiniteWord Dividend, FiniteWord Divisor, FiniteWord& Remainder, FiniteWord& Quotient);

		//
		// LLVM RTTI
		//
		static bool classof(const Value *S) {
			return S->getKind() == VK_FiniteWord;
		}
	};

	namespace finiteword {

		extern const FiniteWord EMPTY;
		extern const FiniteWord ZERO_1BIT;
		extern const FiniteWord ONE_1BIT;

	}

	//
	// GTest
	//
	void PrintTo(const FiniteWord& bar, ::std::ostream* os);

}
