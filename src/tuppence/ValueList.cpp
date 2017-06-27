//===------ ValueList.cpp -------------------------------------------------===//
//
// The Tuppence Programming Language
//
// This file is distributed under the MIT Open Source License.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

#include "ValueList.h"

#include "Logger.h"
#include "RationalWord.h"

#include "llvm/Support/Casting.h"

#include <sstream>
#include <string>
#include <cassert>

namespace tuppence {

	namespace valuelist {

		const FiniteWord EMPTY = FiniteWord::FactoryEmpty();
		const FiniteWord ZERO_1BIT = FiniteWord::FactoryBool((bool)0);
		const FiniteWord ONE_1BIT = FiniteWord::FactoryBool((bool)1);

	}

}

using namespace tuppence;

const std::string ValueList::string() const {
	std::ostringstream strs;
	// iterate reversed for printing
	strs << "(";
	for (auto iter = Vals.rbegin(); iter != Vals.rend(); ++iter) {
		strs << (*iter)->string();
		if (std::next(iter) != Vals.rend()) {
			strs << ", ";
		}
	}
	strs << ")";
	std::string str = strs.str();
	return str;
}

//
// Structural Operations
//

const bool ValueList::isComparable() const {
	for (auto& Val : Vals) {
		if (/*auto FiniteWordVal = */llvm::dyn_cast<FiniteWord>(Val.get())) {
			
		}
		else if (/*auto RationalWordVal = */llvm::dyn_cast<RationalWord>(Val.get())) {

		}
		else if (auto ValueListVal = llvm::dyn_cast<ValueList>(Val.get())) {
			if (!ValueListVal->isComparable()) {
				return false;
			}
		}
		else {
			// some other, non-comparable element
			return false;
		}
	}
	return true;
}

const bool ValueList::operator==(const ValueList other) const {
	auto size = Vals.size();
	assert(size == other.Vals.size() && "Sizes not equal");

	for (auto i = 0; i < size; i++) {
		auto Val = Vals[i];
		auto OtherVal = other.Vals[i];
		if (auto FiniteWordVal = llvm::dyn_cast<FiniteWord>(Val.get())) {
			if (auto OtherFiniteWordVal = llvm::dyn_cast<FiniteWord>(OtherVal.get())) {
				if (*FiniteWordVal != *OtherFiniteWordVal) {
					return false;
				}
			}
			else {
				assert(false && "do better type checking");
				return false;
			}
		}
		else if (auto RationalWordVal = llvm::dyn_cast<RationalWord>(Val.get())) {
			if (auto OtherRationalWordVal = llvm::dyn_cast<RationalWord>(OtherVal.get())) {
				if (*RationalWordVal != *OtherRationalWordVal) {
					return false;
				}
			}
			else {
				assert(false && "do better type checking");
				return false;
			}
		}
		else if (auto ValueListVal = llvm::dyn_cast<ValueList>(Val.get())) {
			if (auto OtherValueListVal = llvm::dyn_cast<ValueList>(OtherVal.get())) {
				if (*ValueListVal != *OtherValueListVal) {
					return false;
				}
			}
			else {
				assert(false && "do better type checking");
				return false;
			}
		}
	}

	return true;
}

const bool ValueList::operator!=(const ValueList other) const {
	auto size = Vals.size();
	assert(size == other.Vals.size() && "Sizes not equal");

	for (auto i = 0; i < size; i++) {
		auto Val = Vals[i];
		auto OtherVal = other.Vals[i];
		if (auto FiniteWordVal = llvm::dyn_cast<FiniteWord>(Val.get())) {
			if (auto OtherFiniteWordVal = llvm::dyn_cast<FiniteWord>(OtherVal.get())) {
				if (*FiniteWordVal == *OtherFiniteWordVal) {
					return false;
				}
			}
			else {
				assert(false && "do better type checking");
				return false;
			}
		}
		else if (auto RationalWordVal = llvm::dyn_cast<RationalWord>(Val.get())) {
			if (auto OtherRationalWordVal = llvm::dyn_cast<RationalWord>(OtherVal.get())) {
				if (*RationalWordVal == *OtherRationalWordVal) {
					return false;
				}
			}
			else {
				assert(false && "do better type checking");
				return false;
			}
		}
		else if (auto ValueListVal = llvm::dyn_cast<ValueList>(Val.get())) {
			if (auto OtherValueListVal = llvm::dyn_cast<ValueList>(OtherVal.get())) {
				if (*ValueListVal == *OtherValueListVal) {
					return false;
				}
			}
			else {
				assert(false && "do better type checking");
				return false;
			}
		}
	}

	return true;
}

const FiniteWord ValueList::equal(std::vector<ValueList> Vals) {
	assert(Vals.size() > 1 && "Vals does not contain more than one element");
	auto Iter = Vals.begin();
	auto First = *Iter;
	Iter++;
	for (; Iter != Vals.end(); ++Iter) {
		// maintain intuitive order
		if (*Iter != First) {
			return valuelist::ZERO_1BIT;
		}
	}
	return valuelist::ONE_1BIT;
}

const FiniteWord ValueList::notequal(std::vector<ValueList> Vals) {
	assert(Vals.size() > 1 && "Vals does not contain more than one element");
	auto Iter = Vals.begin();
	auto First = *Iter;
	Iter++;
	for (; Iter != Vals.end(); ++Iter) {
		// maintain intuitive order
		if (*Iter == First) {
			return valuelist::ZERO_1BIT;
		}
	}
	return valuelist::ONE_1BIT;
}

const std::shared_ptr<Value> ValueList::concatenate() const {
	if (Vals.size() == 0) {
		return LogError("Cannot concatenate 0 elements");
	}
	auto AccumulateFinite = std::make_shared<FiniteWord>();
	auto Iter = Vals.begin();
	for (; Iter != --Vals.end(); ++Iter) {
		auto V = *Iter;
		if (auto FiniteWordIter = llvm::dyn_cast<FiniteWord>(V.get())) {
			AccumulateFinite = std::make_shared<FiniteWord>(FiniteWordIter->concatenate(*AccumulateFinite));
		}
		else {
			return LogError("Expected FiniteWord: " + V->string());
		}
	}

	auto Last = *Iter;
	if (auto FiniteWordLast = llvm::dyn_cast<FiniteWord>(Last.get())) {
		return std::make_shared<FiniteWord>(FiniteWordLast->concatenate(*AccumulateFinite));
	}
	else if (auto RationalWordLast = llvm::dyn_cast<RationalWord>(Last.get())) {
		return std::make_shared<RationalWord>(RationalWordLast->concatenate(*AccumulateFinite));
	}
	else {
		return LogError("Expected FiniteWord or RationalWord: " + Last->string());
	}
}
