#pragma once
#include "cstdlib/cstdint.h"

namespace firefly::kernel::core::gdt {
constexpr static size_t GDT_ENTRIES = 5;

struct access_byte_t {
    // CPU sets to 1 when accessed.
    uint8_t accessed : 1;

    // 1 if read/writable, 0 if not obviously
    uint8_t read_writable : 1;

    // 0 = grows up, 1 = grows down
    // If this is 1 then it can be executed from equal or lower
    // if this is 0 then it can only be executed from `privilege`
    // E.g if this is 0 and privlege is 3 then
    // the kernel wouldn't be able to execute this segment
    uint8_t direction : 1;

    // if it's 1 then it's executable selector, 0 if data selector
    uint8_t executable : 1;

    // Desciptor type, Bit is set
    // if it's a data segment, cleared for system segment
    uint8_t type : 1;

    // Ring level, 0-3
    uint8_t privilege : 2;

    // 1 for all valid sectors
    uint8_t present : 1;
};

struct __attribute__((packed)) flags_t {
    // if 0 then the byte granularity, otherwise the page granularity
    uint8_t granularity : 1;
    // Size bit, 0 if 16 bit protected mode, 1 if 32 bit protected mode.
    uint8_t descriptor_type : 1;
    uint8_t reserved : 2 = 0;
};

struct __attribute__((packed)) segment_descriptor_t {
    uint16_t limit_low;  // 0 - 15
    uint16_t base_low;   // 16 - 31

    uint8_t base_middle;   // 32 - 39
    uint8_t access;  // 40 - 47

    // Maximum addressable unit
    uint8_t granularity;
    uint8_t base_high;       // 56 - 63
};

void init();
}  // namespace firefly::kernel::core::gdt