#pragma once

#include <stdint.h>
#include <stddef.h>

namespace firefly::kernel::core::tss {

typedef struct [[gnu::packed]] {
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
} tss_t;

// See: Intel SDM Figure 7-3. TSS Descriptor
typedef struct [[gnu::packed]]
{
    uint16_t size;
    uint16_t base0;
    uint8_t  base1;
    uint8_t  access;
    uint8_t  flags;
    uint8_t  base2;
    uint32_t base3;
    uint32_t reserved;
} tss_descriptor;

void load_tss(uint16_t gdt_offset);
void core0_tss_init(size_t base);

}  // namespace firefly::kernel::core::tss
