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

	const std::nullptr_t LogError(std::string Str);

	const char LogLexerError(std::string);

	void LogWarning(std::string Str);
}
