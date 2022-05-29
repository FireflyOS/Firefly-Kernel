#pragma once

#include <stdint.h>

static constexpr uint32_t PAGE_SIZE = 4096;
using PhysicalAddress = void *;

enum FillMode : char {
    ZERO = 0,
    NONE = 1  // Don't fill
    // Todo: Asan fill mode?
};