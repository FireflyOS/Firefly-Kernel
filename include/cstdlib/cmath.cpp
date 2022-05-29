#include "cmath.h"

// Method 1
// https://www.codeproject.com/Articles/69941/Best-Square-Root-Method-Algorithm-Function-Precisi
// https://stackoverflow.com/questions/5000109/implement-double-sqrtdouble-x-in-c
// float sqrt(float arg) {
//     union {
//         int i;
//         float x;
//     } u;
//     u.x = arg;
//     u.i = (1 << 29) + (u.i >> 1) - (1 << 22);

//     // Two Babylonian Steps (simplified from:)
//     // u.x = 0.5f * (u.x + x/u.x);
//     // u.x = 0.5f * (u.x + x/u.x);
//     u.x = u.x + arg / u.x;
//     u.x = 0.25f * u.x + arg / u.x;

//     return u.x;
// }

// https://stackoverflow.com/questions/26860574/pow-implementation-in-cmath-and-efficient-replacement
// float powf(float base, int exponent) {
//     float temp;
//     if (! exponent) {
//         return 1;
//     }
//     temp = powf(base, exponent / 2);
//     if ((exponent % 2) == 0) {
//         return temp * temp;
//     } else {
//         if (exponent > 0)
//             return base * temp * temp;
//         else
//             return (temp * temp) / base;
//     }
// }

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

// https://stackoverflow.com/questions/35968963/trying-to-calculate-logarithm-base-10-without-math-h-really-close-just-having
// double ln(double x) {
//     double old_sum = 0.0;
//     double xmlxpl = (x - 1) / (x + 1);
//     double xmlxpl_2 = xmlxpl * xmlxpl;
//     double denom = 1.0;
//     double frac = xmlxpl;
//     double term = frac; // denom start from 1.0
//     double sum = term;

//     while (sum != old_sum) {
//         old_sum = sum;
//         denom += 2.0;
//         frac *= xmlxpl_2;
//         sum += frac / denom;
//     }
//     return 2.0 * sum;
// }

// double log10(double x) { return ln(x) / 2.3025850929940456840179914546844; }

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