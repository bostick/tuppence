
#pragma once

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


uint64_t Math_gcd(uint64_t a, uint64_t b);

uint64_t Math_lcm(uint64_t a, uint64_t b);

uint64_t Math_bitLength(uint64_t n);


}

