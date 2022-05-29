#pragma once

#include <cstdint>
#include <cstddef>

size_t pow(size_t base, size_t exponent);
int log2(uint64_t value);

constexpr inline size_t constexpr_log2(size_t n) {
    return ((n < 2) ? 1 : 1 + constexpr_log2(n / 2));
}