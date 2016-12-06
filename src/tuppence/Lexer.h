//===------ Lexer.h -------------------------------------------------------===//
//
// The Tuppence Programming Language
//
// This file is distributed under the MIT Open Source License.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

#ifndef TUPPENCE_LEXER_H
#define TUPPENCE_LEXER_H

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
		tok_eqeq = -42,
		tok_pp = -43,
		tok_gtgt = -44,
		tok_gtp = -48,
		tok_ddd = -50,
		tok_beq = -51,

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

#endif
