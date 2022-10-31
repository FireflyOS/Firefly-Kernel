#pragma once

#include <cstddef>

extern "C" {
[[noreturn]] extern void enterUserspace(void *addr, size_t stack);
}
