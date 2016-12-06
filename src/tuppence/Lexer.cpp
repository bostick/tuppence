//===------ Lexer.cpp -----------------------------------------------------===//
//
// The Tuppence Programming Language
//
// This file is distributed under the MIT Open Source License.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

#include "Lexer.h"

#include "Logger.h"

#include "llvm/Support/ErrorHandling.h"

#include <cassert>

using namespace tuppence;

Lexer::Lexer(std::istream &In) :
	In(In), LastChar('\0') {}

const std::string tuppence::stringFromToken(char Op) {
	switch (Op) {
	case tok_error: return "tok_error";
	case tok_eqeq: return "==";
	case tok_pp: return "%%";
	case tok_gtgt: return ">>";
	case '\n': return "\\n";
	case EOF: return "EOF";
	case tok_ddd: return "...";
	case tok_for: return "tok_for";
	case tok_beq: return "!=";
	case tok_identifier: return "tok_identifier";
	default:
		if (Op < 0) {
			llvm_unreachable("Add this token");
		}
		return std::string(1, static_cast<char>(Op));
	}
}

const std::string Lexer::currentState() const {
	switch (CurTok) {
	case tok_identifier: return IdentifierStr;
	case tok_rationalword: return RationalWordVal->string();
	case tok_finiteword: return FiniteWordVal->string();
	default: return stringFromToken(CurTok);
	}
}

int Lexer::nextChar() {
	if (Line.empty()) {
		std::getline(In, Line);
		if (!In.good()) {
			return EOF;
		}
		Line = Line + '\n';
	}
	auto first = Line.at(0);
	Line.erase(0, 1);
	return first;
}

char Lexer::gettok() {
    if (LastChar == '\0') {
        LastChar = nextChar();
    }
    
	// Skip any whitespace.
    while (LastChar == ' ' || LastChar == '\t') {
		LastChar = nextChar();
    }
    
	if (isalpha(LastChar) || LastChar == '_') {
		auto Str = std::string();
		Str += LastChar;
		while (isalnum((LastChar = nextChar())) || (LastChar == '_')) {
			Str += LastChar;
		}
		if (Str == "define") {
			return tok_define;
		}
		if (Str == "if") {
			return tok_if;
		}
		if (Str == "else") {
			return tok_else;
		}
		if (Str == "for") {
			return tok_for;
		}
		if (Str == "while") {
			return tok_while;
		}
		if (Str == "var") {
			return tok_var;
		}

		IdentifierStr = Str;
		return tok_identifier;
	}

	if (isdigit(LastChar)) {
		std::string RationalWordStr;
		if (LastChar == '0') {
			// eat any 0 padding
			while (LastChar == '0') {
				LastChar = nextChar();
			}
			if (!isdigit(LastChar)) {
				// 0
				RationalWordStr = "0";
				RationalWordVal = std::make_shared<RationalWord>(RationalWord::FactoryString(RationalWordStr));
				return tok_rationalword;
			}
		}
		// non-0
		do {
			RationalWordStr += LastChar;
			LastChar = nextChar();
		} while (isdigit(LastChar));

		RationalWordVal = std::make_shared<RationalWord>(RationalWord::FactoryString(RationalWordStr));
		return tok_rationalword;
	}

	if (LastChar == '`') {
		std::string FiniteWordStr;
		LastChar = nextChar();
		while (LastChar == ' ') {
			LastChar = nextChar();
		}
		while (LastChar == '0' || LastChar == '1') {
			FiniteWordStr += LastChar;
			LastChar = nextChar();
		}
		if (LastChar == '`') {
			LastChar = nextChar();
			FiniteWordVal = std::make_shared<FiniteWord>(FiniteWord::FactoryString(FiniteWordStr));
			return tok_finiteword;
		}
		else {
			return LogLexerError("Could not lex FiniteWord, problem with " + stringFromToken(LastChar));
		}
	}

	if (LastChar == '=') {
		LastChar = nextChar();
		if (LastChar == '=') {
			LastChar = nextChar();
			return tok_eqeq;
		}
		else {
			return '=';
		}
	}

	if (LastChar == '%') {
		LastChar = nextChar();
		if (LastChar == '%') {
			LastChar = nextChar();
			return tok_pp;
		}
		else {
			return '%';
		}
	}

	if (LastChar == '>') {
		LastChar = nextChar();
		if (LastChar == '>') {
			LastChar = nextChar();
			return tok_gtgt;
		}
		else if (LastChar == '%') {
			LastChar = nextChar();
			return tok_gtp;
		}
		else {
			return '>';
		}
	}

	if (LastChar == '.') {
		LastChar = nextChar();
		if (LastChar == '.') {
			LastChar = nextChar();
			if (LastChar == '.') {
				LastChar = nextChar();
				return tok_ddd;
			}
			else {
				return LogLexerError("Could not lex: ..");
			}
		}
		else {
			return '.';
		}
	}

	if (LastChar == '!') {
		LastChar = nextChar();
		if (LastChar == '=') {
			LastChar = nextChar();
			return tok_beq;
		}
		else {
			return '!';
		}
	}

	if (LastChar == ';') {
		// Comment until end of line.
		do
			LastChar = nextChar();
		while (LastChar != EOF && LastChar != '\n');

		if (LastChar != EOF)
			return gettok();
	}
    
    if (LastChar == '\n') {
        //reset LastChar back to "fresh"
        LastChar = '\0';
        return '\n';
    }
    
	// Check for end of file.  Don't eat the EOF.
    if (LastChar == EOF) {
		return tok_eof;
    }
    
	// Otherwise, just return the character as its ascii value.
	{
		int ThisChar = LastChar;
		LastChar = nextChar();
		return ThisChar;
	}
}

void Lexer::readNextToken() {
    CurTok = gettok();
}

void Lexer::throwAwayLine() {
    Line.clear();
    LastChar = '\0';
}
