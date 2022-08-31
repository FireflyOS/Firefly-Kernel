#pragma once

#include <stdint.h>
#include <stddef.h>

extern "C" void *memset(void *dest, int val, size_t len);
extern "C" void *memcpy(void *dest, const void *src, size_t len);