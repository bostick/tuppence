//===------ FiniteWord.cpp ------------------------------------------------===//
//
// The Tuppence Programming Language
//
// This file is distributed under the MIT Open Source License.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

#include "FiniteWord.h"

#include "llvm/ADT/APInt.h"
#include "llvm/ADT/StringRef.h"

#include <sstream>
#include <vector>

namespace tuppence {

	namespace finiteword {

		const FiniteWord EMPTY = FiniteWord::FactoryEmpty();
		const FiniteWord ZERO_1BIT = FiniteWord::FactoryBool((bool)0);
		const FiniteWord ONE_1BIT = FiniteWord::FactoryBool((bool)1);

	}

}

using namespace tuppence;

/// Default constructor
FiniteWord::FiniteWord() :
	Value(VK_FiniteWord), size(0) {}

FiniteWord::FiniteWord(std::shared_ptr<llvm::APInt> APIntVal) :
	Value(VK_FiniteWord), size(APIntVal->getBitWidth()), Val(APIntVal) {}

const FiniteWord FiniteWord::FactoryEmpty() {
	return FiniteWord();
}

const FiniteWord FiniteWord::FactoryString(std::string BinaryString) {
	auto size = BinaryString.size();
	if (size == 0) {
		return finiteword::EMPTY;
	}

	llvm::StringRef text(BinaryString);
	auto Val = llvm::APInt(static_cast<unsigned int>(size), text, 2);
	return FactoryAPInt(std::make_shared<llvm::APInt>(Val));
}

const FiniteWord FiniteWord::FactoryDecimalString(std::string DecimalString) {
	assert(!DecimalString.empty() && "string is empty");
	assert((DecimalString.size() == 1 || DecimalString[0] != '0') && "string is padded with 0");
	llvm::StringRef text(DecimalString);
	auto needed = llvm::APInt::getBitsNeeded(text, 10);
	auto Val = llvm::APInt(needed, text, 10);
	return FactoryAPInt(std::make_shared<llvm::APInt>(Val));
}

const FiniteWord FiniteWord::FactoryRepsWord(size_t Repetitions, FiniteWord Pattern) {
	auto size = Repetitions * Pattern.size;
	if (size == 0) {
		return finiteword::EMPTY;
	}

	// optimizations for all 0s and all 1s
	{
		auto pop = Pattern.Val->countPopulation();
		if (pop == 0) {
			// all 0s
			auto Accumulate = llvm::APInt(static_cast<unsigned int>(size), (uint64_t)0, false);
			Accumulate.clearAllBits();
			return FiniteWord::FactoryAPInt(std::make_shared<llvm::APInt>(Accumulate));
		}
		if (pop == Pattern.size) {
			// all 1s
			auto Accumulate = llvm::APInt(static_cast<unsigned int>(size), (uint64_t)0, false);
			Accumulate.setAllBits();
			return FiniteWord::FactoryAPInt(std::make_shared<llvm::APInt>(Accumulate));
		}
	}

	auto Width = bitLength(Repetitions);
	auto FiniteWordReps = llvm::APInt(static_cast<unsigned int>(Width), Repetitions, false);
	auto ReversedReps = FiniteWordReps.reverseBits();
	assert(ReversedReps[0] == 1 && "ReversedReps[0] must be 1");
	ReversedReps = ReversedReps.lshr(1);
	auto PatternExtended = Pattern.Val->zextOrSelf(static_cast<unsigned int>(size));
	auto Accumulate = PatternExtended;
	auto AccumulatedSize = Pattern.size;
	while (AccumulatedSize < size) {
		// double the reps
		auto Original = Accumulate;
		Accumulate <<= static_cast<unsigned int>(AccumulatedSize);
		Accumulate |= Original;
		AccumulatedSize *= 2;
		if (ReversedReps[0] == 1) {
			// add 1 rep
			Accumulate <<= static_cast<unsigned int>(Pattern.size);
			Accumulate |= PatternExtended;
			AccumulatedSize += Pattern.size;
		}
		ReversedReps = ReversedReps.lshr(1);
	}
	assert(AccumulatedSize == size && "Accumulated size is not expected size");
	assert(ReversedReps.getActiveBits() == 0 && "ReversedReps is not properly processed");
	return FiniteWord::FactoryAPInt(std::make_shared<llvm::APInt>(Accumulate));
}

const FiniteWord FiniteWord::FactoryBool(bool Bit) {
	auto Val = llvm::APInt(1, (uint64_t)Bit, false);
	return FiniteWord(std::make_shared<llvm::APInt>(Val));
}

const FiniteWord FiniteWord::FactoryAPInt(std::shared_ptr<llvm::APInt> Value) {
	return FiniteWord(Value);
}

const FiniteWord FiniteWord::FactorySizeT(size_t Val) {
	auto A = llvm::APInt(64, Val);
	return FiniteWord::FactoryAPInt(std::make_shared<llvm::APInt>(A));
}

/// Copy constructor
FiniteWord::FiniteWord(const FiniteWord& other) :
	Value(VK_FiniteWord), size(other.size) {
	if (size > 0) {
		Val = other.Val;
	}
}

/// Copy assignment operator
FiniteWord& FiniteWord::operator=(const FiniteWord& other) {
	size = other.size;
	if (size > 0) {
		Val = other.Val;
	}
	return *this;
}

/// Move constructor
FiniteWord::FiniteWord(FiniteWord&& other) :
	Value(VK_FiniteWord), size(other.size) {
	if (size > 0) {
		Val = std::move(other.Val);
	}
}

/// Move assignment operator
FiniteWord& FiniteWord::operator=(FiniteWord&& other) {
	size = other.size;
	if (size > 0) {
		Val = std::move(other.Val);
	}
	return *this;
}

const size_t FiniteWord::getSize() const {
	return size;
}

const std::string FiniteWord::bits() const {
	std::stringstream ss;
	for (auto i = 0; i < size; i++) {
		auto index = size - 1 - i;
		auto bit = (*Val)[static_cast<unsigned int>(index)];
		ss << bit;
	}
	return ss.str();
}

const std::string FiniteWord::decimal() const {
	if (size == 0) {
		return "0";
	} else {
		return Val->toString(10, false);
	}
}

const std::string FiniteWord::decimalSigned() const {
	if (size == 0) {
		return "0";
	}
	else {
		return Val->toString(10, true);
	}
}

const std::string FiniteWord::string() const {
//	return representation();
    std::stringstream ss;
    ss << "`";
    ss << bits();
    ss << "`";
    return ss.str();
}

const bool FiniteWord::get(size_t index) const {
	assert(index < size && "Index is too large");
	return (*Val)[static_cast<unsigned int>(index)];
}

const bool FiniteWord::operator==(const FiniteWord other) const {
	assert(size == other.size && "Sizes not equal");
	if (size == 0) {
		return true;
	}
	return *Val == *other.Val;
}

const bool FiniteWord::operator!=(const FiniteWord other) const {
	assert(size == other.size && "Sizes not equal");
	if (size == 0) {
		return false;
	}
	return *Val != *other.Val;
}

const FiniteWord FiniteWord::rotateRight(size_t amount) const {
	auto Rotated = Val->rotr(static_cast<unsigned int>(amount));
	return FactoryAPInt(std::make_shared<llvm::APInt>(Rotated));
}

const FiniteWord FiniteWord::rotateLeft(size_t amount) const {
	auto Rotated = Val->rotl(static_cast<unsigned int>(amount));
	return FactoryAPInt(std::make_shared<llvm::APInt>(Rotated));
}

const FiniteWord FiniteWord::reverse() const {
	auto Reversed = Val->reverseBits();
	return FactoryAPInt(std::make_shared<llvm::APInt>(Reversed));
}

//
// bitwise operations
//

const FiniteWord FiniteWord::operator~() const {
	if (size == 0) {
		return finiteword::EMPTY;
	}
	auto Not = ~*Val;
	return FactoryAPInt(std::make_shared<llvm::APInt>(Not));
}

const FiniteWord FiniteWord::operator^(FiniteWord other) const {
	assert(size == other.size && "Sizes not equal");
	if (size == 0) {
		return finiteword::EMPTY;
	}
	auto Xor = *Val ^ *other.Val;
	return FactoryAPInt(std::make_shared<llvm::APInt>(Xor));
}

const FiniteWord FiniteWord::operator&(FiniteWord other) const {
	assert(size == other.size && "Sizes not equal");
	if (size == 0) {
		return finiteword::EMPTY;
	}
	auto And = *Val & *other.Val;
	return FactoryAPInt(std::make_shared<llvm::APInt>(And));
}

const FiniteWord FiniteWord::operator|(FiniteWord other) const {
	assert(size == other.size && "Sizes not equal");
	if (size == 0) {
		return finiteword::EMPTY;
	}
	auto Or = *Val | *other.Val;
	return FactoryAPInt(std::make_shared<llvm::APInt>(Or));
}

const uint64_t* FiniteWord::getRawData() const {
	return Val->getRawData();
}

//
// Structural Operations
//

const FiniteWord FiniteWord::equal(std::vector<FiniteWord> Vals) {
	assert(Vals.size() > 1 && "Vals does not contain more than one element");
	auto Iter = Vals.begin();
	auto First = *Iter;
	Iter++;
	for (; Iter != Vals.end(); ++Iter) {
		// maintain intuitive order
		if (*Iter != First) {
			return finiteword::ZERO_1BIT;
		}
	}
	return finiteword::ONE_1BIT;
}

const FiniteWord FiniteWord::notequal(std::vector<FiniteWord> Vals) {
	assert(Vals.size() > 1 && "Vals does not contain more than one element");
	for (auto Iter = Vals.begin(); Iter != Vals.end(); ++Iter) {
		auto First = *Iter;
		auto Iter2 = Iter;
		Iter2++;
		for (; Iter2 != Vals.end(); Iter2++) {
			auto Second = *Iter2;
			// maintain intuitive order
			if (Second == First) {
				return finiteword::ZERO_1BIT;
			}
		}
	}
	return finiteword::ONE_1BIT;
}

const bool FiniteWord::ugt(FiniteWord other) const {
	assert(size == other.size && "Sizes not equal");
	return Val->ugt(*other.Val);
}

const FiniteWord FiniteWord::residue(size_t i) const {
	assert(i <= size && "Index too large");
	if (i == size) {
		return *this;
	}
	if (i == 0) {
		return finiteword::EMPTY;
	}
	auto Trunc = Val->trunc(static_cast<unsigned int>(i));
	return FactoryAPInt(std::make_shared<llvm::APInt>(Trunc));
}

const FiniteWord FiniteWord::shiftRight(size_t i) const {
	assert(i <= size && "Index too large");
	if (i == 0) {
		return *this;
	}
	if (i == size) {
		return FiniteWord::FactoryEmpty();
	}
	auto Shifted = Val->lshr(static_cast<unsigned int>(i)).trunc(static_cast<unsigned int>(size - i));
	return FiniteWord::FactoryAPInt(std::make_shared<llvm::APInt>(Shifted));
}

// this could be passed in as Hi or Lo, so make sure to make
// a copy first
void FiniteWord::shiftRightResidue(size_t i, FiniteWord& Hi, FiniteWord& Lo) const {
	assert(i <= size && "Index too large");
	auto Tmp = *this;
	Hi = Tmp.shiftRight(i);
	Lo = Tmp.residue(i);
}

const FiniteWord FiniteWord::concatenate(FiniteWord R) const {
	if (size == 0) {
		return R;
	}
	if (R.size == 0) {
		return *this;
	}
	auto Res = Val->zextOrSelf(static_cast<unsigned int>(size + R.size));
	auto B = R.Val->zextOrSelf(static_cast<unsigned int>(size + R.size));
	Res <<= static_cast<unsigned int>(R.size);
	Res |= B;
	return FiniteWord::FactoryAPInt(std::make_shared<llvm::APInt>(Res));
}

/// copied from ScalarEvolution.cpp
const FiniteWord FiniteWord::gcd(FiniteWord A, FiniteWord B) {

	auto AVal = *A.Val;
	auto BVal = *B.Val;

	auto ABW = AVal.getBitWidth();
	auto BBW = BVal.getBitWidth();
	
	if (ABW > BBW) {
		BVal = BVal.zext(ABW);
	}
	else if (ABW < BBW) {
		AVal = AVal.zext(BBW);
	}
	
	auto Ret = llvm::APIntOps::GreatestCommonDivisor(AVal, BVal);
	return FiniteWord::FactoryAPInt(std::make_shared<llvm::APInt>(Ret));
}

void FiniteWord::compressPeriod(FiniteWord& period) {
	auto periodSize = period.getSize();
	auto sq = (size_t)sqrt(periodSize);
	for (auto testDivisor = 1; testDivisor <= sq; testDivisor++) {
		if (periodSize % testDivisor == 0) {

			// first use testDivisor
			if (periodSize > 1) {
				if (period.Val->isSplat(testDivisor)) {
					auto modWord = period.residue(testDivisor);
					period = modWord;

					// recurse on compressedPeriod, but it is now smaller
					compressPeriod(period);
					return;
				}
			}

			// then use use periodSize / testDivisor
			if (testDivisor > 1) {
				if (period.Val->isSplat(static_cast<unsigned int>(periodSize / testDivisor))) {
					auto modWord = period.residue(periodSize / testDivisor);
					period = modWord;

					// recurse on compressedPeriod, but it is now smaller
					compressPeriod(period);
					return;
				}
			}
		}
	}
}

const bool FiniteWord::isPeriodCompressed(const FiniteWord period) {
	assert(period.getSize() > 0 && "Period size is 0");

	auto periodSize = period.getSize();
	auto sq = (size_t)sqrt(periodSize);
	for (auto testDivisor = 1; testDivisor <= sq; testDivisor++) {
		if (periodSize % testDivisor == 0) {

			// first use testDivisor
			if (periodSize > 1) {
				if (period.Val->isSplat(testDivisor)) {
					return false;
				}
			}

			// then use periodSize / testDivisor
			if (testDivisor > 1) {
				if (period.Val->isSplat(static_cast<unsigned int>(periodSize / testDivisor))) {
					return false;
				}
			}
		}
	}

	return true;
}

void FiniteWord::multiplyPeriod(FiniteWord& period, size_t factor) {
	period = FiniteWord::FactoryRepsWord(factor, period);
}

//
// Bitwise operations
//

const FiniteWord FiniteWord::bitwise_not(FiniteWord L) {
	return ~L;
}

const FiniteWord FiniteWord::bitwise_or(std::vector<FiniteWord> Vals) {
	assert(Vals.size() > 1 && "Vals does not contain more than one element");
	auto Iter = Vals.begin();
	auto Val = *Iter;
	Iter++;
	for (; Iter != Vals.end(); ++Iter) {
		// maintain intuitive order
		Val = *Iter | Val;
	}
	return Val;
}

const FiniteWord FiniteWord::bitwise_and(std::vector<FiniteWord> Vals) {
	assert(Vals.size() > 1 && "Vals does not contain more than one element");
	auto Iter = Vals.begin();
	auto Val = *Iter;
	Iter++;
	for (; Iter != Vals.end(); ++Iter) {
		// maintain intuitive order
		Val = *Iter & Val;
	}
	return Val;
}

const FiniteWord FiniteWord::bitwise_xor(std::vector<FiniteWord> Vals) {
	assert(Vals.size() > 1 && "Vals does not contain more than one element");
	auto Iter = Vals.begin();
	auto Val = *Iter;
	Iter++;
	for (; Iter != Vals.end(); ++Iter) {
		// maintain intuitive order
		Val = *Iter ^ Val;
	}
	return Val;
}

//
// Arithmetic Operations
//

const FiniteWord FiniteWord::operator-() const {
	assert(size > 0 && "size is 0");
	auto Not = -*Val;
	return FactoryAPInt(std::make_shared<llvm::APInt>(Not));
}

const FiniteWord FiniteWord::operator+(FiniteWord other) const {
	assert(size > 0 && "size is 0");
	assert(size == other.size && "Sizes not equal");
	auto Added = *Val + *other.Val;
	return FactoryAPInt(std::make_shared<llvm::APInt>(Added));
}

const FiniteWord FiniteWord::operator-(FiniteWord other) const {
	assert(size > 0 && "size is 0");
	assert(size == other.size && "Sizes not equal");
	auto Minused = *Val - *other.Val;
	return FactoryAPInt(std::make_shared<llvm::APInt>(Minused));
}

const FiniteWord FiniteWord::operator*(FiniteWord other) const {
	assert(size > 0 && "size is 0");
	assert(size == other.size && "Sizes not equal");
	auto Multiplied = *Val * *other.Val;
	return FactoryAPInt(std::make_shared<llvm::APInt>(Multiplied));
}

const FiniteWord FiniteWord::operator/(FiniteWord other) const {
	assert(size > 0 && "size is 0");
	assert(size == other.size && "Sizes not equal");
	auto Divided = Val->udiv(*other.Val);
	return FactoryAPInt(std::make_shared<llvm::APInt>(Divided));
}

const FiniteWord FiniteWord::operator%(FiniteWord other) const {
	assert(size > 0 && "size is 0");
	assert(size == other.size && "Sizes not equal");
	auto Modded = Val->urem(*other.Val);
	return FactoryAPInt(std::make_shared<llvm::APInt>(Modded));
}

void FiniteWord::plus(std::vector<FiniteWord> Vals, FiniteWord& Hi, FiniteWord& Lo) {
	assert(Vals.size() > 1 && "Vals does not contain more than one element");
	auto Iter = Vals.begin();
	size_t OldWidth = Iter->getSize();
	if (OldWidth == 0) {
		// Still return 1 carry bit, even if the words are empty
		Hi = finiteword::ZERO_1BIT;
		Lo = finiteword::EMPTY;
		return;
	}

	auto CarryWidth = bitLength(Vals.size() - 1);

	size_t NewWidth = OldWidth + CarryWidth;
	auto APVal = Iter->Val->zext(static_cast<unsigned int>(NewWidth));
	Iter++;

	for (; Iter != Vals.end(); ++Iter) {
		assert(Iter->getSize() == OldWidth && "Sizes not equal");
		// maintain intuitive order
		APVal = Iter->Val->zext(static_cast<unsigned int>(NewWidth)) + APVal;
	}

	auto HiAPInt = APVal.lshr(static_cast<unsigned int>(OldWidth)).trunc(static_cast<unsigned int>(CarryWidth));
	Hi = FactoryAPInt(std::make_shared<llvm::APInt>(HiAPInt));
	auto LoAPInt = APVal.trunc(static_cast<unsigned int>(OldWidth));
	Lo = FactoryAPInt(std::make_shared<llvm::APInt>(LoAPInt));
}

void FiniteWord::minus(FiniteWord L, FiniteWord R, FiniteWord& Hi, FiniteWord& Lo) {
	assert(L.size == R.size && "Sizes not equal");
	auto OldWidth = L.size;
	if (OldWidth == 0) {
		// Still return 1 carry bit, even if the words are empty
		Hi = finiteword::ZERO_1BIT;
		Lo = finiteword::EMPTY;
		return;
	}

	auto BorrowWidth = 1;

	size_t NewWidth = OldWidth + BorrowWidth;
	auto NewLVal = L.Val->zext(static_cast<unsigned int>(NewWidth));
	auto NewRVal = R.Val->zext(static_cast<unsigned int>(NewWidth));

	auto NewVal = NewLVal - NewRVal;

	auto HiAPInt = NewVal.lshr(static_cast<unsigned int>(OldWidth)).trunc(static_cast<unsigned int>(BorrowWidth));
	Hi = FactoryAPInt(std::make_shared<llvm::APInt>(HiAPInt));
	auto LoAPInt = NewVal.trunc(static_cast<unsigned int>(OldWidth));
	Lo = FactoryAPInt(std::make_shared<llvm::APInt>(LoAPInt));
}

void FiniteWord::times(std::vector<FiniteWord> Vals, FiniteWord& Hi, FiniteWord& Lo) {
	assert(Vals.size() > 1 && "Vals does not contain more than one element");
	auto Iter = Vals.begin();
	size_t OldWidth = Iter->getSize();
	if (OldWidth == 0) {
		// Still return 1 Hi bit, even if the words are empty
		Hi = finiteword::ZERO_1BIT;
		Lo = finiteword::EMPTY;
		return;
	}

	size_t NewWidth = OldWidth * Vals.size();
	auto APVal = Iter->Val->zext(static_cast<unsigned int>(NewWidth));
	Iter++;

	for (; Iter != Vals.end(); ++Iter) {
		assert(Iter->getSize() == OldWidth && "Sizes not equal");
		// maintain intuitive order
		APVal = Iter->Val->zext(static_cast<unsigned int>(NewWidth)) * APVal;
	}
	auto HiAPInt = APVal.lshr(static_cast<unsigned int>(OldWidth)).trunc(static_cast<unsigned int>(OldWidth * (Vals.size() - 1)));
	Hi = FactoryAPInt(std::make_shared<llvm::APInt>(HiAPInt));
	auto LoAPInt = APVal.trunc(static_cast<unsigned int>(OldWidth));
	Lo = FactoryAPInt(std::make_shared<llvm::APInt>(LoAPInt));
}

void FiniteWord::divide(FiniteWord Dividend, FiniteWord Divisor, FiniteWord& Remainder, FiniteWord& Quotient) {
	assert(Dividend.size == Divisor.size && "Sizes not equal");
	auto OldWidth = Dividend.size;
	if (OldWidth == 0) {
		Quotient = finiteword::EMPTY;
		Remainder = finiteword::EMPTY;
		return;
	}
	llvm::APInt APQuotient;
	llvm::APInt APRemainder;
	llvm::APInt::udivrem(*Dividend.Val, *Divisor.Val, APQuotient, APRemainder);
	Quotient = FactoryAPInt(std::make_shared<llvm::APInt>(APQuotient));
	Remainder = FactoryAPInt(std::make_shared<llvm::APInt>(APRemainder));
}

//
// GTest
//

void tuppence::PrintTo(const FiniteWord& bar, ::std::ostream* os) {
	*os << bar.string();
}
