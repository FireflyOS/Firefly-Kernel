#include "cstdint.h"

inline api_size_t api_strlen(const char *str) {
    api_size_t n = 0;
    while (*str++)
        n++;
    return n;
}

inline api_size_t api_strnlen(const char *str) {
    return api_strlen(str) + 1;
}