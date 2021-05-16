//===------ Logger.h ------------------------------------------------------===//
//
// The Tuppence Programming Language
//
// This file is distributed under the MIT Open Source License.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

#pragma once

#include <cstddef> // for std::nullptr_t
#include <string>

namespace tuppence {

    std::nullptr_t LogError(std::string Str);

	char LogLexerError(std::string);

	void LogWarning(std::string Str);
    
} // end namespace tuppence
