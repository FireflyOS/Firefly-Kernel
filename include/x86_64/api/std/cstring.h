#include "cstdint.h"

inline size_t strlen(const char *str) {
    size_t n = 0;
    while (*str++)
        n++;
    return n;
}

inline size_t strnlen(const char *str) {
    return strlen(str) + 1;
}