//===------ RationalWord.cpp ----------------------------------------------===//
//
// The Tuppence Programming Language
//
// This file is distributed under the MIT Open Source License.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

#include "RationalWord.h"

#include "Logger.h"
#include "TuppenceConfig.h"

#include <cassert>
#include <sstream>
#include <vector>

namespace tuppence {

	namespace rationalword {

		const FiniteWord EMPTY = FiniteWord::FactoryEmpty();
		const FiniteWord ZERO_1BIT = FiniteWord::FactoryBool((bool)0);
		const FiniteWord ONE_1BIT = FiniteWord::FactoryBool((bool)1);

		const RationalWord ZERO = RationalWord::FactoryPeriodTransient(ZERO_1BIT, EMPTY);
		const RationalWord ONE = RationalWord::FactoryPeriodTransient(ZERO_1BIT, ONE_1BIT);
		const RationalWord MINUS_ONE = RationalWord::FactoryPeriodTransient(ONE_1BIT, EMPTY);

	}

}

using namespace tuppence;

// Default constructor
RationalWord::RationalWord() :
	Value(VK_RationalWord), period(rationalword::ZERO_1BIT), transient() {}

RationalWord::RationalWord(FiniteWord Period, FiniteWord Transient) :
	Value(VK_RationalWord), period(Period), transient(Transient) {
	assert(isReduced(period, transient) && "not reduced!");
}

const RationalWord RationalWord::FactoryPeriodTransient(FiniteWord period, FiniteWord transient) {
	reduce(period, transient);
	return RationalWord(period, transient);
}

const RationalWord RationalWord::FactoryString(std::string StrVal) {
	assert(!StrVal.empty() && "string is empty");
	assert((StrVal.size() == 1 || StrVal[0] != '0') && "string is padded with 0");
	auto Transient = FiniteWord::FactoryDecimalString(StrVal);
	return RationalWord::FactoryPeriodTransient(rationalword::ZERO_1BIT, Transient);
}

const RationalWord RationalWord::FactorySizeT(size_t Val) {
	auto Transient = FiniteWord::FactorySizeT(Val);
	return RationalWord::FactoryPeriodTransient(rationalword::ZERO_1BIT, Transient);
}

// Copy constructor
RationalWord::RationalWord(const RationalWord& other) :
	Value(VK_RationalWord), period(other.period), transient(other.transient) {}

// Copy assignment operator
RationalWord& RationalWord::operator=(const RationalWord& rhs) {
	period = rhs.period;
	transient = rhs.transient;
	return *this;
}

// Move constructor
RationalWord::RationalWord(RationalWord&& other) :
	Value(VK_RationalWord), period(other.period), transient(other.transient) {
        // nothing to clear from other
}

// Move assignment operator
RationalWord& RationalWord::operator=(RationalWord&& other) {
	period = other.period;
	transient = other.transient;
	// nothing to clear from other
	return *this;
}

void RationalWord::calculateFraction(RationalWord& Numerator, RationalWord& Denominator) const {
	if (transient.getSize() > 0) {
		auto z = (rationalword::ONE_1BIT.concatenate(FiniteWord::FactoryRepsWord(transient.getSize(), rationalword::ZERO_1BIT)));
		auto xExtended = rationalword::ZERO_1BIT.concatenate(FiniteWord::FactoryRepsWord(transient.getSize(), rationalword::ZERO_1BIT)).concatenate(period);
		auto zExtended = FiniteWord::FactoryRepsWord(period.getSize(), rationalword::ZERO_1BIT).concatenate(z);
		auto xz = xExtended * zExtended;
		if (period.getSize() > 1) {
			auto w = FiniteWord::FactoryRepsWord(period.getSize(), rationalword::ONE_1BIT);
			auto wExtended = rationalword::ZERO_1BIT.concatenate(FiniteWord::FactoryRepsWord(transient.getSize(), rationalword::ZERO_1BIT)).concatenate(w);
			auto yExtended = rationalword::ZERO_1BIT.concatenate(FiniteWord::FactoryRepsWord(period.getSize(), rationalword::ZERO_1BIT)).concatenate(transient);
			auto wy = wExtended * yExtended;
			if (wy.ugt(xz)) {
				auto wy_xz = wy - xz;
				auto divisor = FiniteWord::gcd(wy_xz, wExtended);
				auto wy_xzReduced = wy_xz / divisor;
				auto wReduced = wExtended / divisor;
				//return wy_xzReduced.decimal() + "/" + wReduced.decimal();

				Numerator = RationalWord::FactoryPeriodTransient(rationalword::ZERO_1BIT, wy_xzReduced);
				Denominator = RationalWord::FactoryPeriodTransient(rationalword::ZERO_1BIT, wReduced);
			}
			else {
				auto xz_wy = xz - wy;
				auto divisor = FiniteWord::gcd(xz_wy, wExtended);
				auto xz_wyReduced = xz_wy / divisor;
				auto wReduced = wExtended / divisor;
				//return "-" + xz_wyReduced.decimal() + "/" + wReduced.decimal();

				Numerator = -RationalWord::FactoryPeriodTransient(rationalword::ZERO_1BIT, xz_wyReduced);
				Denominator = RationalWord::FactoryPeriodTransient(rationalword::ZERO_1BIT, wReduced);
			}
		}
		else {
			auto yExtended = rationalword::ZERO_1BIT.concatenate(FiniteWord::FactoryRepsWord(period.getSize(), rationalword::ZERO_1BIT)).concatenate(transient);
			if (yExtended.ugt(xz)) {
				//return (yExtended - xz).decimal();

				Numerator = RationalWord::FactoryPeriodTransient(rationalword::ZERO_1BIT, (yExtended - xz));
				Denominator = rationalword::ONE;
			}
			else {
				//return "-" + (xz - yExtended).decimal();

				Numerator = -RationalWord::FactoryPeriodTransient(rationalword::ZERO_1BIT, (xz - yExtended));
				Denominator = rationalword::ONE;
			}
		}
	}
	else {
		if (period.getSize() > 1) {
			auto w = FiniteWord::FactoryRepsWord(period.getSize(), rationalword::ONE_1BIT);
			
			auto divisor = FiniteWord::gcd(period, w);
			auto xReduced = period / divisor;
			auto wReduced = w / divisor;

			//return "-" + xReduced.decimal() + "/" + wReduced.decimal();

			Numerator = -RationalWord::FactoryPeriodTransient(rationalword::ZERO_1BIT, xReduced);
			Denominator = RationalWord::FactoryPeriodTransient(rationalword::ZERO_1BIT, wReduced);
		}
		else if (period == rationalword::ONE_1BIT) {
			Numerator = rationalword::MINUS_ONE;
			Denominator = rationalword::ONE;
		}
		else {
			Numerator = rationalword::ZERO;
			Denominator = rationalword::ONE;
		}
	}
}

const std::string RationalWord::decimal() const {
	if (isNegativeInteger()) {
		return rationalword::ONE_1BIT.concatenate(transient).decimalSigned();
	}
	if (isNonNegativeInteger()) {
		return transient.decimal();
	}
	RationalWord Numerator;
	RationalWord Denominator;
	calculateFraction(Numerator, Denominator);
	return Numerator.decimal() + "/" + Denominator.decimal();
}

const std::string RationalWord::string() const {
	return decimal();
}

const FiniteWord RationalWord::getPeriod() const {
	return period;
}

const FiniteWord RationalWord::getTransient() const {
	return transient;
}

const bool RationalWord::isNonNegativeInteger() const {
	return period.getSize() == 1 && period == rationalword::ZERO_1BIT;
}

const bool RationalWord::isNegativeInteger() const {
	return period.getSize() == 1 && period == rationalword::ONE_1BIT;
}

const uint64_t RationalWord::getUInt64Value() const {
	return *transient.getRawData();
}

const RationalWord RationalWord::getNumerator() const {
	RationalWord Numerator;
	RationalWord Denominator;
	calculateFraction(Numerator, Denominator);
	return Numerator;
}

const RationalWord RationalWord::getDenominator() const {
	RationalWord Numerator;
	RationalWord Denominator;
	calculateFraction(Numerator, Denominator);
	return Denominator;
}

void RationalWord::windupTransient(FiniteWord& period, FiniteWord& transient) {
	assert(period.getSize() > 0 && "Period size is 0");
	while (1) {
		auto min = std::min(period.getSize(), transient.getSize());
		auto common = 0;
		for (auto i = 0; i < min; i++) {
			if (period.get(period.getSize() - 1 - i) == transient.get(transient.getSize() - 1 - i)) {
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
		period = period.rotateLeft(common);

		//remove last bit of transient;
		transient = transient.residue(transient.getSize() - common);
	}
}

void RationalWord::reduce(FiniteWord& period, FiniteWord& transient) {
	assert(period.getSize() > 0 && "Period size is 0");
	FiniteWord::compressPeriod(period);
	windupTransient(period, transient);
}

const bool RationalWord::isReduced(FiniteWord period, FiniteWord transient) {
	assert(period.getSize() > 0 && "Period size is 0");
	// transient is wound up?
	if (transient.getSize() > 0) {
		auto lastPeriodBit = period.get(period.getSize() - 1);
		auto lastTransientBit = transient.get(transient.getSize() - 1);
		if (lastPeriodBit == lastTransientBit) {
			return false;
		}
	}

	if (!FiniteWord::isPeriodCompressed(period)) {
		return false;
	}

	return true;
}

// extend transient to be size extent, unwind period to accomodate
void RationalWord::extendTransient(FiniteWord& period, FiniteWord& transient, size_t extent) {
	assert(extent >= transient.getSize() && "Extent is too small");
	auto diff = extent - transient.getSize();

	if (diff == 0) {
		return;
	}

	auto fullRepetitions = diff / period.getSize();
	auto leftOver = diff % period.getSize();

	auto NewTransient = period.residue(leftOver);
	NewTransient = NewTransient.concatenate(FiniteWord::FactoryRepsWord(fullRepetitions, period));
	NewTransient = NewTransient.concatenate(transient);

	transient = NewTransient;
	period = period.rotateRight(diff);
}

void RationalWord::alignPeriodAndTransient(
	FiniteWord& PeriodL, FiniteWord& TransientL,
	FiniteWord& PeriodR, FiniteWord& TransientR) {

	// Extend transients to match
	if (TransientL.getSize() > TransientR.getSize()) {
		extendTransient(PeriodR, TransientR, TransientL.getSize());
	}
	else if (TransientL.getSize() < TransientR.getSize()) {
		extendTransient(PeriodL, TransientL, TransientR.getSize());
	}

	// Multiply Periods
	auto LCM = lcm(PeriodL.getSize(), PeriodR.getSize());
	auto ToMultiplyL = LCM / PeriodL.getSize();
	auto ToMultiplyR = LCM / PeriodR.getSize();
	FiniteWord::multiplyPeriod(PeriodL, ToMultiplyL);
	FiniteWord::multiplyPeriod(PeriodR, ToMultiplyR);
}

const RationalWord RationalWord::operator~() const {
	return RationalWord::FactoryPeriodTransient(~period, ~transient);
}

const RationalWord RationalWord::operator|(RationalWord other) const {
	auto PeriodA = period;
	auto PeriodB = other.period;
	auto TransientA = transient;
	auto TransientB = other.transient;

	alignPeriodAndTransient(PeriodA, TransientA, PeriodB, TransientB);

	auto Period = PeriodA | PeriodB;
	auto Transient = TransientA | TransientB;

	return RationalWord::FactoryPeriodTransient(Period, Transient);
}

const RationalWord RationalWord::operator&(RationalWord other) const {
	auto PeriodA = period;
	auto PeriodB = other.period;
	auto TransientA = transient;
	auto TransientB = other.transient;

	alignPeriodAndTransient(PeriodA, TransientA, PeriodB, TransientB);

	auto Period = PeriodA & PeriodB;
	auto Transient = TransientA & TransientB;

	return RationalWord::FactoryPeriodTransient(Period, Transient);
}

const RationalWord RationalWord::operator^(RationalWord other) const {
	auto PeriodA = period;
	auto PeriodB = other.period;
	auto TransientA = transient;
	auto TransientB = other.transient;

	alignPeriodAndTransient(PeriodA, TransientA, PeriodB, TransientB);

	auto Period = PeriodA ^ PeriodB;
	auto Transient = TransientA ^ TransientB;

	return RationalWord::FactoryPeriodTransient(Period, Transient);
}

//
// Structural Operations
//

const bool RationalWord::operator==(RationalWord other) const {
	return period.getSize() == other.period.getSize() && transient.getSize() == other.transient.getSize() &&
		period == other.period && transient == other.transient;
}

const bool RationalWord::operator!=(RationalWord other) const {
	return period.getSize() != other.period.getSize() || transient.getSize() != other.transient.getSize() ||
		period != other.period || transient != other.transient;
}

const FiniteWord RationalWord::equal(std::vector<RationalWord> Vals) {
	assert(Vals.size() > 1 && "Vals does not contain more than one element");
	auto Iter = Vals.begin();
	auto First = *Iter;
	Iter++;
	for (; Iter != Vals.end(); ++Iter) {
		// maintain intuitive order
		if (*Iter != First) {
			return rationalword::ZERO_1BIT;
		}
	}
	return rationalword::ONE_1BIT;
}

const FiniteWord RationalWord::notequal(std::vector<RationalWord> Vals) {
	assert(Vals.size() > 1 && "Vals does not contain more than one element");
	for (auto Iter = Vals.begin(); Iter != Vals.end(); ++Iter) {
		auto First = *Iter;
		auto Iter2 = Iter;
		Iter2++;
		for (; Iter2 != Vals.end(); Iter2++) {
			auto Second = *Iter2;
			// maintain intuitive order
			if (Second == First) {
				return rationalword::ZERO_1BIT;
			}
		}
	}
	return rationalword::ONE_1BIT;
}

const FiniteWord RationalWord::residue(size_t i) const {
	if (i <= transient.getSize()) {
		return transient.residue(i);
	}

	auto forPeriod = i - transient.getSize();
	auto fullRepetitions = forPeriod / period.getSize();
	auto leftOver = forPeriod % period.getSize();

	return period.residue(leftOver).concatenate(FiniteWord::FactoryRepsWord(fullRepetitions, period)).concatenate(transient);
}

const RationalWord RationalWord::shiftRight(size_t i) const {
	if (i <= transient.getSize()) {
		auto Shifted = transient.shiftRight(i);
		return RationalWord::FactoryPeriodTransient(period, Shifted);
	}

	auto forPeriod = i - transient.getSize();
	auto leftOver = forPeriod % period.getSize();

	auto NewPeriod = period.rotateRight(leftOver);
	FiniteWord NewTransient;
	return RationalWord::FactoryPeriodTransient(NewPeriod, NewTransient);
}

// It is possible to pass this is as Hi or Lo, so make sure
// to make a copy first.
void RationalWord::shiftRightResidue(size_t i, RationalWord& Hi, FiniteWord& Lo) const {
	auto Tmp = *this;
	Hi = Tmp.shiftRight(i);
	Lo = Tmp.residue(i);
}

const RationalWord RationalWord::concatenate(FiniteWord R) const {
	auto T = transient.concatenate(R);
	return RationalWord::FactoryPeriodTransient(period, T);
}

//
// Bitwise operations
//

const RationalWord RationalWord::bitwise_not(RationalWord L) {
	return ~L;
}

const RationalWord RationalWord::bitwise_or(std::vector<RationalWord> Vals) {
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

const RationalWord RationalWord::bitwise_and(std::vector<RationalWord> Vals) {
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

const RationalWord RationalWord::bitwise_xor(std::vector<RationalWord> Vals) {
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

const RationalWord RationalWord::operator-() const {
	static auto ONE = RationalWord::FactoryString("1");
	return ~(*this) + ONE;
}

const RationalWord RationalWord::operator+(RationalWord other) const {
	
	auto PeriodA = period;
	auto PeriodB = other.period;
	auto TransientA = transient;
	auto TransientB = other.transient;

	alignPeriodAndTransient(PeriodA, TransientA, PeriodB, TransientB);

	FiniteWord TransientOutput;
	FiniteWord TransientCarry;

	// maintain intuitive order
	std::vector<FiniteWord> TransientVals = { TransientB, TransientA };
	FiniteWord::plus(TransientVals, TransientCarry, TransientOutput);
	assert(TransientCarry.getSize() == 1 && "Adding 2 FiniteWords did not give a Carry of size 1");

	FiniteWord PeriodOutput;
	FiniteWord PeriodCarry(TransientCarry);
	FiniteWord PreviousPeriodCarry;
	std::vector<FiniteWord> PeriodVals;

	FiniteWord TransientAccumulate(TransientOutput);

	do {
		PreviousPeriodCarry = PeriodCarry;

		auto CarryExtended = FiniteWord::FactoryRepsWord(PeriodA.getSize() - 1, rationalword::ZERO_1BIT).concatenate(PeriodCarry);

		PeriodVals.clear();
		// maintain intuitive order
		PeriodVals.push_back(PeriodB);
		PeriodVals.push_back(PeriodA);
		PeriodVals.push_back(CarryExtended);
		FiniteWord::plus(PeriodVals, PeriodCarry, PeriodOutput);
		assert(PeriodCarry.getSize() == 2 && "Adding 3 FiniteWords did not give a Carry of size 2");
		PeriodCarry = PeriodCarry.residue(1);

		TransientAccumulate = PeriodOutput.concatenate(TransientAccumulate);

	} while (PeriodCarry != PreviousPeriodCarry);

	return RationalWord::FactoryPeriodTransient(PeriodOutput, TransientAccumulate);
}

const RationalWord RationalWord::operator-(RationalWord other) const {
	auto PeriodA = period;
	auto PeriodB = ~other.period;
	auto TransientA = transient;
	auto TransientB = ~other.transient;

	alignPeriodAndTransient(PeriodA, TransientA, PeriodB, TransientB);

	FiniteWord TransientOutput;
	FiniteWord TransientBorrow;

	auto BorrowExtended = FiniteWord::FactoryRepsWord(TransientA.getSize() - 1, rationalword::ZERO_1BIT).concatenate(rationalword::ONE_1BIT);
	std::vector<FiniteWord> TransientVals = { TransientA, TransientB, BorrowExtended };
	FiniteWord::plus(TransientVals, TransientBorrow, TransientOutput);
	assert(TransientBorrow.getSize() == 2 && "Adding 3 FiniteWords did not give a Carry of size 2");
	TransientBorrow = TransientBorrow.residue(1);

	FiniteWord PeriodOutput;
	FiniteWord PeriodBorrow(TransientBorrow);
	FiniteWord PreviousPeriodBorrow;
	std::vector<FiniteWord> PeriodVals;

	FiniteWord TransientAccumulate(TransientOutput);

	do {
		PreviousPeriodBorrow = PeriodBorrow;

		auto BorrowExtended = FiniteWord::FactoryRepsWord(PeriodA.getSize() - 1, rationalword::ZERO_1BIT).concatenate(PeriodBorrow);

		PeriodVals.clear();
		PeriodVals.push_back(PeriodA);
		PeriodVals.push_back(PeriodB);
		PeriodVals.push_back(BorrowExtended);
		FiniteWord::plus(PeriodVals, PeriodBorrow, PeriodOutput);
		assert(PeriodBorrow.getSize() == 2 && "Adding 3 FiniteWords did not give a Carry of size 2");
		PeriodBorrow = PeriodBorrow.residue(1);

		TransientAccumulate = PeriodOutput.concatenate(TransientAccumulate);

	} while (PeriodBorrow != PreviousPeriodBorrow);

	return RationalWord::FactoryPeriodTransient(PeriodOutput, TransientAccumulate);
}

RationalWord finiteMultiply(RationalWord A, FiniteWord B) {
	auto BReversed = B.reverse();
	RationalWord Accumulate;
	FiniteWord Bit;
	while (BReversed.getSize() > 0) {
		Accumulate = Accumulate.concatenate(rationalword::ZERO_1BIT);
		BReversed.shiftRightResidue(1, BReversed, Bit);
		if (Bit == rationalword::ONE_1BIT) {
			Accumulate = Accumulate + A;
		}
	}
	return Accumulate;
}

const RationalWord RationalWord::operator*(RationalWord other) const {

	auto A = *this;
	auto B = other;

	auto BTransientProduct = finiteMultiply(A, B.getTransient());
	auto BPeriodProduct = finiteMultiply(A, B.getPeriod());

	auto BTransientSize = B.getTransient().getSize();
	auto BPeriodSize = B.getPeriod().getSize();

	FiniteWord Period;
	FiniteWord Transient;

	RationalWord Partial;
	BTransientProduct.shiftRightResidue(BTransientSize, Partial, Transient);

	std::vector<RationalWord> PeriodPartials;

	while (1) {
		Partial = BPeriodProduct + Partial;

		auto PeriodStart = std::find(PeriodPartials.begin(), PeriodPartials.end(), Partial);
		if (PeriodStart != PeriodPartials.end()) {
			for (auto TransientIter = PeriodPartials.begin(); TransientIter < PeriodStart; TransientIter++) {
				Transient = TransientIter->residue(BPeriodSize).concatenate(Transient);
			}
			for (auto PeriodIter = PeriodStart; PeriodIter < PeriodPartials.end(); PeriodIter++) {
				Period = PeriodIter->residue(BPeriodSize).concatenate(Period);
			}

			break;
		}

		PeriodPartials.push_back(Partial);
		if (BTransientSize + PeriodPartials.size() >= Tuppence_LOOP_LIMIT) {
			LogWarning("Loop limit exceeded in RationalWord multiply. Returning truncated result. Loop limit is: " + std::to_string(Tuppence_LOOP_LIMIT));
			for (auto TransientIter = PeriodPartials.begin(); TransientIter < PeriodPartials.end(); TransientIter++) {
				Transient = TransientIter->residue(1).concatenate(Transient);
			}
			return FactoryPeriodTransient(rationalword::ZERO_1BIT, Transient);
		}

		Partial = Partial.shiftRight(BPeriodSize);
	}

	return FactoryPeriodTransient(Period, Transient);
}

const RationalWord RationalWord::operator/(RationalWord other) const {
	assert(other.residue(1) == rationalword::ONE_1BIT && "Divisor must be odd!");

	const auto A = *this;
	const auto B = other;

	auto Partial = A;

	FiniteWord Transient;
	FiniteWord Period;

	std::vector<RationalWord> Partials;

	Partials.push_back(Partial);

	while (1) {
		if (Partial.residue(1) == rationalword::ONE_1BIT) {
			Partial = Partial - B;
		}
		Partial = Partial.shiftRight(1);

		auto PeriodStart = std::find(Partials.begin(), Partials.end(), Partial);
		if (PeriodStart != Partials.end()) {
			for (auto TransientIter = Partials.begin(); TransientIter < PeriodStart; TransientIter++) {
				Transient = TransientIter->residue(1).concatenate(Transient);
			}
			for (auto PeriodIter = PeriodStart; PeriodIter < Partials.end(); PeriodIter++) {
				Period = PeriodIter->residue(1).concatenate(Period);
			}

			break;
		}

		Partials.push_back(Partial);
		if (Partials.size() >= Tuppence_LOOP_LIMIT) {
			LogWarning("Loop limit exceeded in RationalWord divide. Returning truncated result. Loop limit is: " + std::to_string(Tuppence_LOOP_LIMIT));
			for (auto TransientIter = Partials.begin(); TransientIter < Partials.end(); TransientIter++) {
				Transient = TransientIter->residue(1).concatenate(Transient);
			}
			return FactoryPeriodTransient(rationalword::ZERO_1BIT, Transient);
		}
	}

	return FactoryPeriodTransient(Period, Transient);
}

const RationalWord RationalWord::plus(std::vector<RationalWord> Vals) {
	assert(Vals.size() > 1 && "Vals does not contain more than one element");
	auto Iter = Vals.begin();
	auto Val = *Iter;
	Iter++;
	for (; Iter != Vals.end(); ++Iter) {
		// maintain intuitive order
		Val = *Iter + Val;
	}
	return Val;
}

const RationalWord RationalWord::minus(RationalWord L, RationalWord R) {
	return L - R;
}

const RationalWord RationalWord::times(std::vector<RationalWord> Vals) {
	assert(Vals.size() > 1 && "Vals does not contain more than one element");
	auto Iter = Vals.begin();
	auto Val = *Iter;
	Iter++;
	for (; Iter != Vals.end(); ++Iter) {
		// maintain intuitive order
		Val = *Iter * Val;
	}
	return Val;
}

const RationalWord RationalWord::divide(RationalWord L, RationalWord R) {
	return L / R;
}

//
// GTest
//

void tuppence::PrintTo(const RationalWord& bar, ::std::ostream* os) {
	*os << bar.string();
}
