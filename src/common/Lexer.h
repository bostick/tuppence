//===------ Lexer.h -------------------------------------------------------===//
//
// The Tuppence Programming Language
//
// This file is distributed under the MIT Open Source License.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

#pragma once

#include <memory>
#include <string>
#include <istream>








enum Token : char {
    // tok_eof is equal to EOF
    tok_eof = -1,
    tok_error = -2,
    
    // commands
    tok_define = -10,
//    tok_command = -11,
    
    // primary
    tok_identifier = -20,
    tok_integerword = -21,
    tok_finiteword = -22,
    
    // control
    tok_if = -30,
    tok_else = -32,
//    tok_for = -33,
    tok_while = -35,
    
    // function
    tok_function = -40,
    
    // operators
    tok_equal_equal = -52,
    tok_percent_percent = -53,
    tok_greater_greater = -54,
    tok_greater_percent = -58,
    tok_dot_dot_dot = -60,
    tok_bang_equal = -61,
    tok_star_star = -62,
    tok_slash_percent = -63
    
};


std::string stringFromToken(char);



namespace tuppence {

	class Lexer {
		std::istream &In;
		char LastChar;
		int nextChar();
		std::string Line;
		char gettok();

	public:
		Lexer(std::istream &);

		std::string IdentifierStr;
        std::string IntegerWordStr;
        std::string FiniteWordStr;

		const std::string currentState() const;

		char CurTok;
		void readNextToken();
		void throwAwayLine();
	};
    
    char LogLexerError(std::string);
    
} // end namespace tuppence
