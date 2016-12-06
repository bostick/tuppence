//===------ RationalWord.test.cpp -----------------------------------------===//
//
// The Tuppence Programming Language
//
// This file is distributed under the MIT Open Source License.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

#include "RationalWord.h"

#include "gtest/gtest.h"

namespace tuppence {
	namespace rationalword {

		RationalWord TWO;
		RationalWord THREE;
		RationalWord FOUR;
		RationalWord FIVE;
		RationalWord SEVEN;
		RationalWord TEN;
		RationalWord SEVENTEEN;
		RationalWord NINETEEN;
		RationalWord TWENTYTHREE;
		RationalWord THIRTYTHREE;
		RationalWord TWOHUNDREDNINETEEN;
		RationalWord THREEHUNDREDTHIRTYTHREE;
		RationalWord TENTHOUSANDONE;

		RationalWord SEVENTEEN_OVER_SEVEN;
		RationalWord FIVE_OVER_SEVENTEEN;
		RationalWord NINETEEN_OVER_SEVEN;
		RationalWord FIVE_OVER_NINETEEN;
		RationalWord TWENTYTHREE_OVER_THREE;
		RationalWord FIVE_OVER_TWENTRYTHREE;
		RationalWord TWENTYTHREE_OVER_SEVEN;
		RationalWord ONE_OVER_TWOHUNDREDNINETEEN;
		RationalWord ONE_OVER_TENTHOUSANDONE;

	}
}

using namespace tuppence;

class RationalWordTest : public ::testing::Test {
protected:

	RationalWordTest() {}

	virtual ~RationalWordTest() {}

	static void SetUpTestCase() {
	
		rationalword::TWO = RationalWord::FactoryString("2");
		rationalword::THREE = RationalWord::FactoryString("3");
		rationalword::FOUR = RationalWord::FactoryString("4");
		rationalword::FIVE = RationalWord::FactoryString("5");
		rationalword::SEVEN = RationalWord::FactoryString("7");
		rationalword::TEN = RationalWord::FactoryString("10");
		rationalword::SEVENTEEN = RationalWord::FactoryString("17");
		rationalword::NINETEEN = RationalWord::FactoryString("19");
		rationalword::TWENTYTHREE = RationalWord::FactoryString("23");
		rationalword::THIRTYTHREE = RationalWord::FactoryString("33");
		rationalword::TWOHUNDREDNINETEEN = RationalWord::FactoryString("219");
		rationalword::THREEHUNDREDTHIRTYTHREE = RationalWord::FactoryString("333");
		rationalword::TENTHOUSANDONE = RationalWord::FactoryString("10001");
		
		rationalword::SEVENTEEN_OVER_SEVEN = rationalword::SEVENTEEN / rationalword::SEVEN;
		rationalword::FIVE_OVER_SEVENTEEN = rationalword::FIVE / rationalword::SEVENTEEN;
		rationalword::NINETEEN_OVER_SEVEN = rationalword::NINETEEN / rationalword::SEVEN;
		rationalword::FIVE_OVER_NINETEEN = rationalword::FIVE / rationalword::NINETEEN;
		rationalword::TWENTYTHREE_OVER_THREE = rationalword::TWENTYTHREE / rationalword::THREE;
		rationalword::FIVE_OVER_TWENTRYTHREE = rationalword::FIVE / rationalword::TWENTYTHREE;
		rationalword::TWENTYTHREE_OVER_SEVEN = rationalword::TWENTYTHREE / rationalword::SEVEN;
		rationalword::ONE_OVER_TWOHUNDREDNINETEEN = rationalword::ONE / rationalword::TWOHUNDREDNINETEEN;
		rationalword::ONE_OVER_TENTHOUSANDONE = rationalword::ONE / rationalword::TENTHOUSANDONE;
	
	}

	virtual void SetUp() {}

	virtual void TearDown() {}
};

TEST_F(RationalWordTest, multiply) {

	EXPECT_EQ("-3", (rationalword::MINUS_ONE * rationalword::THREE).decimal());
	
	EXPECT_EQ("5/7", (rationalword::FIVE_OVER_SEVENTEEN * rationalword::SEVENTEEN_OVER_SEVEN).decimal());

	EXPECT_EQ("5/7", (rationalword::SEVENTEEN_OVER_SEVEN * rationalword::FIVE_OVER_SEVENTEEN).decimal());
	
	EXPECT_EQ("5/7", (rationalword::NINETEEN_OVER_SEVEN * rationalword::FIVE_OVER_NINETEEN).decimal());
	
	EXPECT_EQ("5/7", (rationalword::FIVE_OVER_NINETEEN * rationalword::NINETEEN_OVER_SEVEN).decimal());
	
	EXPECT_EQ("5/3", (rationalword::TWENTYTHREE_OVER_THREE * rationalword::FIVE_OVER_TWENTRYTHREE).decimal());
	
	EXPECT_EQ("5/7", (rationalword::TWENTYTHREE_OVER_SEVEN * rationalword::FIVE_OVER_TWENTRYTHREE).decimal());
	
	EXPECT_EQ("5/7", (rationalword::FIVE_OVER_TWENTRYTHREE * rationalword::TWENTYTHREE_OVER_SEVEN).decimal());
	
	EXPECT_EQ("1", (rationalword::ONE_OVER_TWOHUNDREDNINETEEN * rationalword::TWOHUNDREDNINETEEN).decimal());
	
	EXPECT_EQ("1", (rationalword::TWOHUNDREDNINETEEN * rationalword::ONE_OVER_TWOHUNDREDNINETEEN).decimal());
	
	EXPECT_EQ("1", (rationalword::ONE * rationalword::ONE).decimal());
	
	EXPECT_EQ("2", (rationalword::TWO * rationalword::ONE).decimal());
	
	EXPECT_EQ("3", (rationalword::THREE * rationalword::ONE).decimal());
	
	EXPECT_EQ("4", (rationalword::FOUR * rationalword::ONE).decimal());
	
	EXPECT_EQ("5", (rationalword::FIVE * rationalword::ONE).decimal());
	
	EXPECT_EQ("10", (rationalword::TEN * rationalword::ONE).decimal());
	
	EXPECT_EQ("33", (rationalword::THIRTYTHREE * rationalword::ONE).decimal());
	
	EXPECT_EQ("219", (rationalword::TWOHUNDREDNINETEEN * rationalword::ONE).decimal());
	
	EXPECT_EQ("333", (rationalword::THREEHUNDREDTHIRTYTHREE * rationalword::ONE).decimal());
}

TEST_F(RationalWordTest, divide) {

	EXPECT_EQ("17/7", rationalword::SEVENTEEN_OVER_SEVEN.decimal());

	EXPECT_EQ("5/17", rationalword::FIVE_OVER_SEVENTEEN.decimal());

	EXPECT_EQ("19/7", rationalword::NINETEEN_OVER_SEVEN.decimal());

	EXPECT_EQ("5/19", rationalword::FIVE_OVER_NINETEEN.decimal());

	EXPECT_EQ("23/3", rationalword::TWENTYTHREE_OVER_THREE.decimal());

	EXPECT_EQ("5/23", rationalword::FIVE_OVER_TWENTRYTHREE.decimal());

	EXPECT_EQ("23/7", rationalword::TWENTYTHREE_OVER_SEVEN.decimal());

	EXPECT_EQ("1/219", rationalword::ONE_OVER_TWOHUNDREDNINETEEN.decimal());

	EXPECT_EQ("1/10001", rationalword::ONE_OVER_TENTHOUSANDONE.decimal());
}

TEST_F(RationalWordTest, unequal) {

	auto ZERO_1BIT = FiniteWord::FactoryBool((bool)0);

	std::vector<RationalWord> Vals;
	Vals.push_back(rationalword::ONE);
	Vals.push_back(rationalword::ONE);
	Vals.push_back(rationalword::TWO);
	EXPECT_EQ(ZERO_1BIT, RationalWord::notequal(Vals));


	Vals.clear();
	Vals.push_back(rationalword::TWO);
	Vals.push_back(rationalword::ONE);
	Vals.push_back(rationalword::ONE);
	EXPECT_EQ(ZERO_1BIT, RationalWord::notequal(Vals));
}

TEST_F(RationalWordTest, subtract) {
	
	EXPECT_EQ(rationalword::ONE, rationalword::TWO - rationalword::ONE);

	EXPECT_EQ(rationalword::TWO, rationalword::THREE - rationalword::ONE);

	EXPECT_EQ(rationalword::TWO, rationalword::SEVEN - rationalword::FIVE);
}

TEST_F(RationalWordTest, windupTransient) {

	FiniteWord period = FiniteWord::FactoryString("11011");
	FiniteWord transient = FiniteWord::FactoryString("11011");

	auto Res = RationalWord::FactoryPeriodTransient(period, transient);
	period = Res.getPeriod();
	transient = Res.getTransient();

	EXPECT_EQ(FiniteWord::FactoryString("11011"), period);
	EXPECT_EQ(FiniteWord::FactoryString(""), transient);

}
