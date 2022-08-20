#pragma once

#include <stdint.h>

static constexpr uint64_t HIGH_VMA = 0xFFFF800000000000UL;
static constexpr uint64_t KERNEL_CODE_VMA = 0xFFFFFFFF80000000UL;

static constexpr uint32_t PAGE_SIZE = 4096;
static constexpr uint32_t PAGE_SHIFT = 12;

using PhysicalAddress = void *;

enum FillMode : char {
    ZERO = 0,
    NONE = 1  // Don't fill
};