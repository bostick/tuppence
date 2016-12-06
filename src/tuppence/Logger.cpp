//===------ Logger.cpp ----------------------------------------------------===//
//
// The Tuppence Programming Language
//
// This file is distributed under the MIT Open Source License.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

#include "Logger.h"

#include "Lexer.h"

#include <iostream>

using namespace tuppence;

const std::nullptr_t tuppence::LogError(std::string Str) {
    std::cerr << "Error: " << Str << "\n";
    return nullptr;
}

const char tuppence::LogLexerError(std::string Str) {
	std::cerr << "Syntax Error: " << Str << "\n";
	return tok_error;
}

void tuppence::LogWarning(std::string Str) {
	std::cerr << "Warning: " << Str << "\n";
}
