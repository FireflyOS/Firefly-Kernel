#pragma once

#include <stddef.h>
#include <stdint.h>

#include <firefly/compiler/compiler.hpp>

namespace firefly::kernel {
struct CpuData;
}

namespace firefly::kernel::core::tss {

struct Tss {
    uint32_t reserved0;
    uint64_t RSP0;
    uint64_t RSP1;
    uint64_t RSP2;

    uint32_t reserved1;
    uint32_t reserved2;

    uint64_t IST1;
    uint64_t IST2;
    uint64_t IST3;
    uint64_t IST4;
    uint64_t IST5;
    uint64_t IST6;
    uint64_t IST7;

    uint32_t reserved3;
    uint32_t reserved4;

    uint16_t IOBP;
} PACKED;

// See: Intel SDM Figure 7-3. TSS Descriptor
struct descriptor {
    uint16_t size;
    uint16_t base0;
    uint8_t base1;
    uint8_t access;
    uint8_t flags;
    uint8_t base2;
    uint32_t base3;
    uint32_t reserved;
} PACKED;

void init(CpuData* cpudata, uint64_t stack);

}  // namespace firefly::kernel::core::tss
