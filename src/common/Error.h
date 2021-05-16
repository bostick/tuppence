
#pragma once

#include "TuppenceValue.h"

#include <cstdint>

#ifdef _WIN32
#    if RUNTIME_DLL
#    define RUNTIME_API __declspec(dllexport)
#    else
#    define RUNTIME_API __declspec(dllimport)
#endif
#else
#define RUNTIME_API
#endif

extern "C" RUNTIME_API {

	
	
}
