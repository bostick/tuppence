//===------ ValueList.h - ValueList class definition ----------------------===//
//
// The Tuppence Programming Language
//
// This file is distributed under the MIT Open Source License.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

#ifndef TUPPENCE_VALUELIST_H
#define TUPPENCE_VALUELIST_H

#include "FiniteWord.h"
#include "Value.h"

#include <string>

namespace tuppence {

	class ValueList : public Value {
		const std::vector<std::shared_ptr<Value>> Vals;

	public:
		ValueList(std::vector<std::shared_ptr<Value>> Vals) :
			Value(VK_ValueList), Vals(Vals) {}

		const std::string string() const override;

		typedef std::vector<std::shared_ptr<Value>>::const_iterator const_iterator;

		const_iterator begin() const { return Vals.begin(); }
		const_iterator end() const { return Vals.end(); }

		const std::shared_ptr<Value> operator[](size_t i) const {
			return Vals[i];
		}

		const size_t size() const {
			return Vals.size();
		}

		//
		// structural operations
		//

		/// This list only contains comparable elements
		const bool isComparable() const;

		const bool operator==(ValueList) const;
		const bool operator!=(ValueList) const;

		static const FiniteWord equal(std::vector<ValueList>);
		static const FiniteWord notequal(std::vector<ValueList>);

		const std::shared_ptr<Value> concatenate() const;

		//
		// LLVM RTTI
		//

		static bool classof(const Value *S) {
			return S->getKind() == VK_ValueList;
		}
	};
}

#endif
