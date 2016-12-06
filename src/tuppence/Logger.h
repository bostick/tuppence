//===------ Logger.h ------------------------------------------------------===//
//
// The Tuppence Programming Language
//
// This file is distributed under the MIT Open Source License.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

#ifndef TUPPENCE_LOGGER_H
#define TUPPENCE_LOGGER_H

#include <string>

namespace tuppence {

	const std::nullptr_t LogError(std::string Str);

	const char LogLexerError(std::string);

	void LogWarning(std::string Str);
}

#endif
