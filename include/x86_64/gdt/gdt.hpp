#pragma once

#include <stdint.h>

#include "x86_64/compiler/clang++.hpp"
#include "x86_64/gdt/tss.hpp"

#define GDT_MAX_ENTRIES 5
#define NULENT 0 /* null descriptor entry */
#define CS_KRN 1
#define DS_KRN 2
#define CS_USR 3
#define DS_USR 4

namespace firefly::kernel::core::gdt {
extern "C" void load_gdt(uint64_t);

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
    uint64_t base;
} PACKED;

class GDT {
public:
    gdtd_t gdtd[GDT_MAX_ENTRIES];
    tss::tss_descriptor tssd;
    size_t registered_entries;
} PACKED;

enum SELECTOR : uint16_t
{
    KRN_NULL = 0x0,
    KRN_CS,
    KRN_DS,
    USR_CS,
    USR_DS
};

class GDTconfig {
public:
    void set_tss(uint64_t base, uint8_t flags, uint8_t access);  // Used for: TSS
    void set(int offset, uint8_t flags, uint8_t access);         // Used for: NULL, CS, DS
};
void init();

uint16_t ltr_entry_offset() noexcept;
uint16_t gdt_entry_offset(enum SELECTOR selector) noexcept;

}  // namespace firefly::kernel::core::gdt
