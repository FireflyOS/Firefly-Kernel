#pragma once

#include <cstddef>
#include <cstdint>

namespace firefly::libkern::cstring {
size_t strlen(const char *str);
char *strcpy(char *dest, const char *src);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *str1, const char *str2, size_t n);
char *strchr(const char *str, int c);
char *strchrn(const char *str, int c, int n);
int toupper(char c);
char *strtok(char *s, const char *delimiters);
}  // namespace firefly::libkern::cstring