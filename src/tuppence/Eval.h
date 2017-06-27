//===------ Eval.h --------------------------------------------------------===//
//
// The Tuppence Programming Language
//
// This file is distributed under the MIT Open Source License.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

#pragma once

#include "Value.h"

#include <map>

namespace tuppence {

	namespace eval {

		extern std::map<std::string, std::shared_ptr<Value>> NamedValues;

		//
		// Functions
		//

		const std::shared_ptr<Value> print(std::vector<std::shared_ptr<Value>> Args);
		const std::shared_ptr<Value> exit(std::vector<std::shared_ptr<Value>> Args);

		/// Convert a FiniteWord into a RationalWord 
		const std::shared_ptr<Value> rationalize(std::vector<std::shared_ptr<Value>> Args);

		extern const Symbol SYMBOL_INFINITY;
		extern const BuiltinFunction BUILTIN_PRINT;
		extern const BuiltinFunction BUILTIN_EXIT;
		extern const BuiltinFunction BUILTIN_RATIONALIZE;
	}
}
