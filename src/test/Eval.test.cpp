//===------ Eval.test.cpp -------------------------------------------------===//
//
// The Tuppence Programming Language
//
// This file is distributed under the MIT Open Source License.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

#include "Parser.h"

#include "llvm/ADT/APInt.h"
#include "llvm/ADT/StringRef.h"

#include "gtest/gtest.h"

using namespace tuppence;

TEST(Eval, Test1) {

	{
		std::stringstream ss("`11011`\n");
		Parser P(ss);
		P.readNextToken();
		auto Parsed = P.ParseTopLevelExpression();
		auto Expr = llvm::dyn_cast<FiniteWordExprAST>(Parsed.get());
		ASSERT_TRUE(Expr != nullptr);
		auto Evaled = Expr->eval();
		auto Val = llvm::dyn_cast<FiniteWord>(Evaled.get());
		ASSERT_TRUE(Val != nullptr);

		EXPECT_EQ("`11011`", Val->representation());
	}

	{
		std::stringstream ss("27\n");
		Parser P(ss);
		P.readNextToken();
		auto Parsed = P.ParseTopLevelExpression();
		auto Expr = llvm::dyn_cast<RationalWordExprAST>(Parsed.get());
		ASSERT_TRUE(Expr != nullptr);
		auto Evaled = Expr->eval();
		auto Val = llvm::dyn_cast<RationalWord>(Evaled.get());
		ASSERT_TRUE(Val != nullptr);

		EXPECT_EQ("0'11011", Val->representation());

		auto Nine = std::make_shared<RationalWord>(RationalWord::FactoryString("9"));

		auto Res = Val->residue(*Nine->getTransient().getRawData());
		EXPECT_EQ("`000011011`", Res.representation());
	}

	{
		std::stringstream ss("0#`11011`\n");
		Parser P(ss);
		P.readNextToken();
		auto E = P.ParseTopLevelExpression();
		ASSERT_TRUE(E != nullptr);
		auto Expr = llvm::dyn_cast<InfixExprAST>(E.get());
		ASSERT_TRUE(Expr != nullptr);
		EXPECT_EQ(Expr->getOp(), '#');
		auto Evaled = Expr->eval();
		ASSERT_TRUE(Evaled != nullptr);
		auto Val = llvm::dyn_cast<RationalWord>(Evaled.get());
		ASSERT_TRUE(Val != nullptr);

		EXPECT_EQ("0'11011", Val->representation());
	}

	// Eval code
	{
		std::stringstream ss("17 / ``\n");
		Parser P(ss);
		P.readNextToken();
		auto E = P.ParseTopLevelExpression();
		ASSERT_TRUE(E != nullptr);
		auto Expr = llvm::dyn_cast<BinaryExprAST>(E.get());
		ASSERT_TRUE(Expr != nullptr);

		auto LHS = Expr->getLHS();
		ASSERT_TRUE(LHS != nullptr);
		auto RationalWordLHS = llvm::dyn_cast<RationalWordExprAST>(LHS.get());
		ASSERT_TRUE(RationalWordLHS != nullptr);

		auto RHS = Expr->getRHS();
		ASSERT_TRUE(RHS != nullptr);
		auto FiniteWordRHS = llvm::dyn_cast<FiniteWordExprAST>(RHS.get());
		ASSERT_TRUE(FiniteWordRHS != nullptr);

		auto Evaled = Expr->eval();
		ASSERT_TRUE(Evaled == nullptr);
	}

	// Test Eval code
	{
		std::stringstream ss("17 - ``\n");
		Parser P(ss);
		P.readNextToken();
		auto E = P.ParseTopLevelExpression();
		ASSERT_TRUE(E != nullptr);
		auto Expr = llvm::dyn_cast<BinaryExprAST>(E.get());
		ASSERT_TRUE(Expr != nullptr);

		auto LHS = Expr->getLHS();
		ASSERT_TRUE(LHS != nullptr);
		auto RationalWordLHS = llvm::dyn_cast<RationalWordExprAST>(LHS.get());
		ASSERT_TRUE(RationalWordLHS != nullptr);

		auto RHS = Expr->getRHS();
		ASSERT_TRUE(RHS != nullptr);
		auto FiniteWordRHS = llvm::dyn_cast<FiniteWordExprAST>(RHS.get());
		ASSERT_TRUE(FiniteWordRHS != nullptr);

		auto Evaled = Expr->eval();
		ASSERT_TRUE(Evaled == nullptr);
	}

	// Test Eval code
	{
		std::stringstream ss("`` / ``\n");
		Parser P(ss);
		P.readNextToken();
		auto E = P.ParseTopLevelExpression();
		ASSERT_TRUE(E != nullptr);
		auto Expr = llvm::dyn_cast<BinaryExprAST>(E.get());
		ASSERT_TRUE(Expr != nullptr);

		auto LHS = Expr->getLHS();
		ASSERT_TRUE(LHS != nullptr);
		auto FiniteWordLHS = llvm::dyn_cast<FiniteWordExprAST>(LHS.get());
		ASSERT_TRUE(FiniteWordLHS != nullptr);

		auto RHS = Expr->getRHS();
		ASSERT_TRUE(RHS != nullptr);
		auto FiniteWordRHS = llvm::dyn_cast<FiniteWordExprAST>(RHS.get());
		ASSERT_TRUE(FiniteWordRHS != nullptr);

		auto Evaled = Expr->eval();
		ASSERT_TRUE(Evaled == nullptr);
	}
}

TEST(Eval, ErrorTest1) {

	{
		std::stringstream ss("() # 2\n");
		Parser P(ss);
		P.readNextToken();
		auto Parsed = P.ParseTopLevelExpression();
		ASSERT_TRUE(Parsed != nullptr);
		auto Evaled = Parsed->eval();
		ASSERT_TRUE(Evaled == nullptr);
	}
}
