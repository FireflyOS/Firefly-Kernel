#pragma once

#include <stddef.h>
#include <stdint.h>

extern "C" void *memset(void *dest, int val, size_t len);
extern "C" void *memcpy(void *dest, const void *src, size_t len);
int memcmp(const char *s1, const char *s2, int n);