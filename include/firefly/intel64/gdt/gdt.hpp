#pragma once

#include <stdint.h>

#include "firefly/compiler/compiler.hpp"
#include "firefly/intel64/gdt/tss.hpp"

static constexpr int8_t GDT_MAX_ENTRIES{ 5 };

enum SegmentSelector : int8_t {
    null,
    kernCS,
    kernDS,
    userCS,
    userDS,
    tss
};

namespace firefly::kernel::core::gdt {
extern "C" void load_gdt(uint64_t);

struct Gdtr {
    uint16_t size;
    uint64_t base;
} PACKED;

struct Gdt {
    struct PACKED {
        uint16_t limit{ 0 };
        uint16_t base0{ 0 };
        uint8_t base1{ 0 };
        uint8_t access;
        uint8_t flags;
        uint8_t base2{ 0 };
    } gdtDescriptors[GDT_MAX_ENTRIES];

    tss::descriptor tssDescriptors;
} PACKED;

void init(Gdt &gdt);
void setGdtEntry(Gdt &cpuGdt, uint64_t selectorIdx, uint8_t flags, uint8_t access);
void setTssEntry(Gdt &cpuGdt, uint64_t selectorIdx, uint8_t flags, uint8_t access);

uint16_t tssEntryOffset();
uint16_t gdtEntryOffset(SegmentSelector selector);
}  // namespace firefly::kernel::core::gdt
