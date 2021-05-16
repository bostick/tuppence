//===------ Value.test.cpp ------------------------------------------------===//
//
// The Tuppence Programming Language
//
// This file is distributed under the MIT Open Source License.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

#include "tuppence/Value.h"

#include "gtest/gtest.h"

using namespace tuppence;

TEST(Value, bitLength) {
	EXPECT_EQ(1, bitLength(1));
	EXPECT_EQ(2, bitLength(3));
	EXPECT_EQ(3, bitLength(7));
	EXPECT_EQ(4, bitLength(15));
	EXPECT_EQ(8, bitLength(255));
}
