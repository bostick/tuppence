//===------ FiniteWord.test.cpp -------------------------------------------===//
//
// The Tuppence Programming Language
//
// This file is distributed under the MIT Open Source License.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

#include "FiniteWord.h"

#include "gtest/gtest.h"

using namespace tuppence;

class FiniteWordTest : public ::testing::Test {
protected:

	FiniteWordTest() {}

	virtual ~FiniteWordTest() {}

	static void SetUpTestCase() {}

	virtual void SetUp() {}

	virtual void TearDown() {}
};

TEST_F(FiniteWordTest, unequal) {
	std::vector<FiniteWord> Vals;
	Vals.push_back(finiteword::ZERO_1BIT);
	Vals.push_back(finiteword::ZERO_1BIT);
	Vals.push_back(finiteword::ONE_1BIT);
	EXPECT_EQ(finiteword::ZERO_1BIT, FiniteWord::notequal(Vals));


	Vals.clear();
	Vals.push_back(finiteword::ONE_1BIT);
	Vals.push_back(finiteword::ZERO_1BIT);
	Vals.push_back(finiteword::ZERO_1BIT);
	EXPECT_EQ(finiteword::ZERO_1BIT, FiniteWord::notequal(Vals));
}

TEST_F(FiniteWordTest, concatenate) {

	auto Res = finiteword::ONE_1BIT.concatenate(finiteword::ONE_1BIT);

	EXPECT_EQ("11", Res.bits());

	EXPECT_EQ("`11`", Res.string());
}

TEST_F(FiniteWordTest, period) {

	EXPECT_TRUE(FiniteWord::isPeriodCompressed(FiniteWord::FactoryString("101")));

	EXPECT_FALSE(FiniteWord::isPeriodCompressed(FiniteWord::FactoryString("1010")));

	auto A = FiniteWord::FactoryString("1010");
	FiniteWord::compressPeriod(A);
	EXPECT_EQ(FiniteWord::FactoryString("10"), A);

}

TEST_F(FiniteWordTest, FactoryRepsWord) {

	auto Pattern = FiniteWord::FactoryString("1110");

	auto A = FiniteWord::FactoryRepsWord(1, Pattern);
	EXPECT_EQ(FiniteWord::FactoryString("1110"), A);

	auto B = FiniteWord::FactoryRepsWord(2, Pattern);
	EXPECT_EQ(FiniteWord::FactoryString("11101110"), B);

	auto C = FiniteWord::FactoryRepsWord(3, Pattern);
	EXPECT_EQ(FiniteWord::FactoryString("111011101110"), C);

	auto D = FiniteWord::FactoryRepsWord(10, Pattern);
	EXPECT_EQ(FiniteWord::FactoryString("1110111011101110111011101110111011101110"), D);
}
