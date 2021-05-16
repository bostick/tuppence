//===------ Lexer.cpp -----------------------------------------------------===//
//
// The Tuppence Programming Language
//
// This file is distributed under the MIT Open Source License.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

#include "../common/Lexer.h"

// #include "tuppence/Logger.h"

#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

using namespace tuppence;

char tuppence::LogLexerError(std::string Msg) {
    llvm::errs() << "Syntax Error: " << Msg << "\n";
    return tok_error;
}

Lexer::Lexer(std::istream &In) :
	In(In), LastChar('\0') {}

std::string stringFromToken(char Op) {
    switch (Op) {
        
        // properly escape for printing
        case '\n': return "'\\n'";
        
        case tok_eof: return "tok_eof";
        case tok_error: return "tok_error";
        
        case tok_define: return "tok_define";
            
        case tok_identifier: return "tok_identifier";
        case tok_integerword: return "tok_integerword";
        case tok_finiteword: return "tok_finiteword";
            
        case tok_if: return "tok_if";
        case tok_else: return "tok_else";
//        case tok_for: return "tok_for";
        case tok_while: return "tok_while";
            
        case tok_equal_equal: return "==";
        case tok_percent_percent: return "%%";
        case tok_greater_greater: return ">>";
        case tok_greater_percent: return ">%";
        case tok_dot_dot_dot: return "...";
        case tok_bang_equal: return "!=";
        case tok_star_star: return "**";
        case tok_slash_percent: return "/%";
            
        default:
            if (Op < 0) {
                llvm::errs() << "Add this token: " << static_cast<int>(Op) << "\n";
                assert(false && "Add this token");
            }
            return std::string("'") + Op + "'";
    }
}

const std::string Lexer::currentState() const {
	switch (CurTok) {
	case tok_identifier: return IdentifierStr;
	case tok_integerword: return IntegerWordStr;
	case tok_finiteword: return FiniteWordStr;
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
    
//    if (LastChar == ':') {
//        LastChar = nextChar();
//        return tok_command;
//    }
    
	if (isalpha(LastChar) || LastChar == '_') {
		auto Str = std::string();
		Str += LastChar;
		while (isalnum((LastChar = nextChar())) || (LastChar == '_')) {
			Str += LastChar;
		}
        if (Str == "define") {
            return tok_define;
        } else if (Str == "if") {
			return tok_if;
		} else if (Str == "else") {
			return tok_else;
//        } else if (Str == "for") {
//            return tok_for;
		} else if (Str == "while") {
			return tok_while;
        } else if (Str == "function") {
            return tok_function;
        }

		IdentifierStr = Str;
		return tok_identifier;
	}

	if (isdigit(LastChar)) {
        IntegerWordStr.clear();
        
		if (LastChar == '0') {
			// eat any 0 padding
			while (LastChar == '0') {
				LastChar = nextChar();
			}
			if (!isdigit(LastChar)) {
				// 0
				IntegerWordStr = "0";
				return tok_integerword;
			}
		}
		// non-0
		do {
			IntegerWordStr += LastChar;
			LastChar = nextChar();
		} while (isdigit(LastChar));

		return tok_integerword;
	}

	if (LastChar == '`') {
        FiniteWordStr.clear();
        
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
			return tok_finiteword;
		}
		else {
            return LogLexerError(std::string("Could not lex FiniteWord, problem with ") + stringFromToken(LastChar));
		}
	}

	if (LastChar == '=') {
		LastChar = nextChar();
		if (LastChar == '=') {
			LastChar = nextChar();
			return tok_equal_equal;
		}
		else {
			return '=';
		}
	}

	if (LastChar == '%') {
		LastChar = nextChar();
		if (LastChar == '%') {
			LastChar = nextChar();
			return tok_percent_percent;
		}
		else {
			return '%';
		}
	}

	if (LastChar == '>') {
		LastChar = nextChar();
		if (LastChar == '>') {
			LastChar = nextChar();
			return tok_greater_greater;
		}
		else if (LastChar == '%') {
			LastChar = nextChar();
			return tok_greater_percent;
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
				return tok_dot_dot_dot;
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
			return tok_bang_equal;
		}
		else {
			return '!';
		}
	}

    if (LastChar == '*') {
        LastChar = nextChar();
        if (LastChar == '*') {
            LastChar = nextChar();
            return tok_star_star;
        }
        else {
            return '*';
        }
    }
    
    if (LastChar == '/') {
        LastChar = nextChar();
        if (LastChar == '%') {
            LastChar = nextChar();
            return tok_slash_percent;
        }
        else {
            return '/';
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

//char LogLexerError(std::string Str) {
//    llvm::errs() << "Syntax Error: " << Str << "\n";
//    return tok_error;
//}
