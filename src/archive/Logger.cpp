//===------ Logger.cpp ----------------------------------------------------===//
//
// The Tuppence Programming Language
//
// This file is distributed under the MIT Open Source License.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

#include "tuppence/Logger.h"

#include "tuppence/Lexer.h"

#include "llvm/Support/raw_ostream.h"

using namespace tuppence;

std::nullptr_t tuppence::LogError(std::string Str) {
    llvm::errs() << "Error: " << Str << "\n";
    return nullptr;
}

char tuppence::LogLexerError(std::string Str) {
	llvm::errs() << "Syntax Error: " << Str << "\n";
	return tok_error;
}

void tuppence::LogWarning(std::string Str) {
	llvm::errs() << "Warning: " << Str << "\n";
}
