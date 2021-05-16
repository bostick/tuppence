
#include "../common/Logger.h"

#include "llvm/Support/raw_ostream.h"

void LogWarning(const char *msg) {
	llvm::errs() << "Warning: " << msg << "\n";
}
