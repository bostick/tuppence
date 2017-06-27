//===------ Lexer.h -------------------------------------------------------===//
//
// The Tuppence Programming Language
//
// This file is distributed under the MIT Open Source License.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

#pragma once

#include "FiniteWord.h"
#include "RationalWord.h"

#include <memory>
#include <string>

namespace tuppence {

	enum Token {
		tok_eof = -1,
		tok_error = -2,

		// commands
		tok_define = -10,

		// primary
		tok_identifier = -20,
		tok_rationalword = -21,
		tok_finiteword = -22,

		// control
		tok_if = -30,
		tok_else = -32,
		tok_for = -33,
		tok_while = -35,

		// operators
		tok_equal_equal = -42,
		tok_percent_percent = -43,
		tok_greater_greater = -44,
		tok_greater_percent = -48,
		tok_dot_dot_dot = -50,
		tok_bang_equal = -51,
        tok_star_star = -52,

		// var definition
		tok_var = -60
	};

	const std::string stringFromToken(char);

	class Lexer {
		std::istream &In;
		char LastChar;
		int nextChar();
		std::string Line;
		char gettok();

	public:
		Lexer(std::istream &);

		std::string IdentifierStr;
		std::shared_ptr<RationalWord> RationalWordVal;
		std::shared_ptr<FiniteWord> FiniteWordVal;

		const std::string currentState() const;

		char CurTok;
		void readNextToken();
		void throwAwayLine();
	};
}
