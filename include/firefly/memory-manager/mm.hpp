#pragma once

#include <cstdint>

enum AddressLayout : uint64_t {
    // clang-format off
    PageData = 0xFFFF'D000'0000'0000UL,
    // SlabHeap = 0xFFFF'B000'0000'0000UL,
    High     = 0xFFFF'8000'0000'0000UL,
    Code     = 0xFFFF'FFFF'8000'0000UL,
    Low      = 0x0000'0000'0000'0000UL
    // clang-format on
};

enum PageSize : uint32_t {
    Size4K = 0x00001000,
    Size2M = 0x00200000,
    Size1G = 0x40000000,
};

static constexpr uint32_t PAGE_SHIFT = 12;  // Lower 12 bits of a virtual address denote the offset in the page frame
using PhysicalAddress = void *;
using VirtualAddress = void *;

enum FillMode : char {
    ZERO = 0,
    NONE = 1  // Don't fill
};
