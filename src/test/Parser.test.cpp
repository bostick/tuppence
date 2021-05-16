//===------ Parser.test.cpp -----------------------------------------------===//
//
// The Tuppence Programming Language
//
// This file is distributed under the MIT Open Source License.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

#include "tuppence/Parser.h"
#include "tuppence/RationalWord.h"

#include "gtest/gtest.h"

#include "llvm/Support/Casting.h"

using namespace tuppence;

TEST(Parser, Test1) {
	std::stringstream ss;
	ss << "1+1\n";
	Parser P(ss);
	P.readNextToken();
	auto Parsed = P.ParseTopLevelExpression();
    auto Def = llvm::dyn_cast<DefinitionAST>(Parsed.get());
    auto Body = Def->getBody();
	auto Expr = llvm::dyn_cast<InfixExprAST>(Body.get());
	EXPECT_TRUE(Expr != nullptr);
}

TEST(Parser, Test2) {
	std::stringstream ss;
	ss << "() # 2\n";
	Parser P(ss);
	P.readNextToken();
	auto Parsed = P.ParseTopLevelExpression();
    auto Def = llvm::dyn_cast<DefinitionAST>(Parsed.get());
    auto Body = Def->getBody();
	auto Expr = llvm::dyn_cast<InfixExprAST>(Body.get());
	EXPECT_TRUE(Expr != nullptr);
}

TEST(Parser, Test3) {
	std::stringstream ss;
	ss << "3 --\n";
	Parser P(ss);
	P.readNextToken();
	auto Parsed = P.ParseTopLevelExpression();
	EXPECT_TRUE(Parsed == nullptr);
}

TEST(Parser, Test4) {
	std::stringstream ss;
	ss << "01\n";
	Parser P(ss);
	P.readNextToken();
	auto Parsed = P.ParseTopLevelExpression();
    auto Def = llvm::dyn_cast<DefinitionAST>(Parsed.get());
    auto Body = Def->getBody();
	auto Expr = llvm::dyn_cast<IntegerWordExprAST>(Body.get());
	EXPECT_TRUE(Expr != nullptr);
//    auto Val = Expr->eval();
//    auto RationalWordVal = llvm::dyn_cast<RationalWord>(Val.get());
//    EXPECT_TRUE(RationalWordVal != nullptr);
//
//    EXPECT_EQ("1", RationalWordVal->decimal());
}
