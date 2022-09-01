#include "cmath.h"

size_t pow(size_t base, size_t exponent) {
    size_t temp;
    if (! exponent) {
        return 1;
    }
    temp = pow(base, exponent / 2);
    if ((exponent % 2) == 0) {
        return temp * temp;
    } else {
        if (exponent > 0)
            return base * temp * temp;
        else
            return (temp * temp) / base;
    }
}

// https://stackoverflow.com/questions/11376288/fast-computing-of-log2-for-64-bit-integers
int log2(uint64_t value)
{
    static const int tab64[64] = {
        63,  0, 58,  1, 59, 47, 53,  2,
        60, 39, 48, 27, 54, 33, 42,  3,
        61, 51, 37, 40, 49, 18, 28, 20,
        55, 30, 34, 11, 43, 14, 22,  4,
        62, 57, 46, 52, 38, 26, 32, 41,
        50, 36, 17, 19, 29, 10, 13, 21,
        56, 45, 25, 31, 35, 16,  9, 12,
        44, 24, 15,  8, 23,  7,  6,  5};
    value |= value >> 1;
    value |= value >> 2;
    value |= value >> 4;
    value |= value >> 8;
    value |= value >> 16;
    value |= value >> 32;
    return tab64[((uint64_t)((value - (value >> 1))*0x07EDD5E59A4E28C2)) >> 58];
}