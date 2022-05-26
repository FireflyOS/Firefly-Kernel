#pragma once

#include <cxxshim/cstddef>
#include <stdarg.h>

namespace firefly::libkern::fmt {
int vsnprintf(char* str, size_t size, const char* fmt, va_list ap);
int printf(const char* fmt, ...);
void itoa(size_t num, char* str, int base);
int atoi(const char* str);
char* strrev(char* str);
}  // namespace firefly::libkern::fmt