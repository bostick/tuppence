
#include "../common/TuppenceMath.h"

uint64_t Math_gcd(uint64_t a, uint64_t b) {
    while (1) {
        if (a == 0) {
            return b;
        }
        b %= a;
        if (b == 0) {
            return a;
        }
        a %= b;
    }
}

uint64_t Math_lcm(uint64_t a, uint64_t b) {
    auto temp = Math_gcd(a, b);
    if (temp) {
        return a / temp * b;
    }
    else {
        return 0;
    }
}

uint64_t Math_bitLength(uint64_t n) {
    auto r = 0;
    while (n > 0) {
        r++;
        n = n >> 1;
    }
    return r;
}
