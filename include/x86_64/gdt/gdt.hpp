#pragma once

#include <stdint.h>

#define PACKED __attribute__((packed))
#define GDT_MAX_ENTRIES 5
#define NULENT 0 /* null descriptor entry */
#define CS_KRN 1
#define DS_KRN 2
#define CS_USR 3
#define DS_USR 4

namespace firefly::kernel::core::gdt {
class gdtd_t {
public:
    uint16_t limit;
    uint16_t base0;
    uint8_t base1;
    uint8_t access;
    uint8_t flags;
    uint8_t base2;
} PACKED;

class gdtr_t {
public:
    uint16_t size;
    uint64_t offset;
} PACKED;

class GDTconfig {
public:
    void set(int offset, uint8_t flags, uint8_t access);
};
void init();
}  // namespace firefly::kernel::core::gdt
