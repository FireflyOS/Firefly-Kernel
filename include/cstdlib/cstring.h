#pragma once
#include "cstdint.h"

inline size_t strlen(const char* str) {
    size_t _sz{};
    while (str[_sz++]);
    return _sz;
}

inline size_t strnlen(const char* str) {
    return strlen(str) + 1;
}

// inline size_t strlen(const vga_char* str) {
//     size_t _sz{};
//     while (str[_sz++].codepoint);
//     return _sz;
// }

// inline size_t strnlen(const vga_char* str) {
//     return strlen(str) + 1;
// }