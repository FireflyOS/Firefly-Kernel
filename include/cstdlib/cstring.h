#pragma once

#include <cstdint>
#include <cstddef>

inline size_t strlen(const char *str) {
    size_t n = 0;
    while (*str++)
        n++;
    return n;
}

inline size_t strnlen(const char *str) {
    return strlen(str) + 1;
}

extern "C" void *memset(void *s, char c, int n);
extern int memcmp(const char *s1, const char *s2, int n);
extern char *strcpy(char *dest, const char *src);
extern int strcmp(const char *s1, const char *s2);
extern char *strchr(const char *str, int c);
extern char *strchrn(const char *str, int c, int n);
extern int toupper(char c);
extern char *strtok(char *s, const char *delimiters);
extern int digitcount(uint32_t num);
extern "C" void memcpy(void* dest, const void* src, size_t count);