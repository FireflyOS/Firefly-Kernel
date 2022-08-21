#pragma once

#include <stdint.h>

enum AddressLayout : uint64_t {
    High = 0xFFFF800000000000UL,
    Code = 0xFFFFFFFF80000000UL,
    Low = 0x0000000000000000UL
};

static constexpr uint32_t PAGE_SIZE = 4096;
static constexpr uint32_t PAGE_SHIFT = 12;  // Lower 12 bits of a virtual address denote the offset in the page frame
using PhysicalAddress = void *;
using VirtualAddress = void *;

enum FillMode : char {
    ZERO = 0,
    NONE = 1  // Don't fill
};