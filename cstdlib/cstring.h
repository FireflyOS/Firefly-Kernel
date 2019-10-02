#pragma once
#include "cstdint.h"

size_t strlen(const char* chr) {
    size_t _sz{};
    while (chr[_sz++]);
    return _sz;
}

size_t strnlen(const char* chr) {
    return strlen(chr) + 1;
}

size_t strlen(const uint16_t* chr) {
    size_t _sz{};
    while (chr[_sz++]);
    return _sz;
}

size_t strnlen(const uint16_t* chr) {
    return strlen(chr) + 1;
}