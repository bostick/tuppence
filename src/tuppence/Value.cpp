//===------ Value.cpp ------------------------------------------------------===//
//
// The Tuppence Programming Language
//
// This file is distributed under the MIT Open Source License.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

#include "Value.h"

#include <cassert>
#include <cstddef> // for size_t
#include <string>

using namespace tuppence;

const std::string tuppence::stringFromValueKind(Value::ValueKind Kind) {
	switch (Kind) {
	case Value::ValueKind::VK_BuiltinFunction: return "BuiltinFunction";
	case Value::ValueKind::VK_FiniteWord: return "FiniteWord";
	case Value::ValueKind::VK_RationalWord: return "RationalWord";
	case Value::ValueKind::VK_UserFunction: return "UserFunction";
	case Value::ValueKind::VK_ValueList: return "ValueList";
	default: {
		assert(false && "ADD THIS KIND");
		return "ASSERT";
	}
	}
}

const std::string BuiltinFunction::string() const {
	return "<BuiltinFunction:" + Name + ">";
}

const std::string UserFunction::string() const {
	return "<UserFunction:" + Name + ">";
}

const bool Symbol::operator==(const Symbol other) const {
	return Name == other.Name;
}

const std::string Symbol::string() const {
	return Name;
}

//
// Math
//

const size_t tuppence::gcd(size_t a, size_t b) {
	while (1) {
		if (a == 0) {
			return b;
		}
		b %= a;
		if (b == 0) {
			return a;
		}
		a %= b;
	}
}

const size_t tuppence::lcm(size_t a, size_t b) {
	auto temp = gcd(a, b);
	if (temp) {
		return a / temp * b;
	}
	else {
		return (size_t)0;
	}
}

const size_t tuppence::bitLength(size_t n) {
	auto r = 0;
	while (n > 0) {
		r++;
		n = n >> 1;
	}
	return r;
}
