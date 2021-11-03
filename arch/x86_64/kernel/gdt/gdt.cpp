#include <stl/cstdlib/stdio.h>

#include "x86_64/gdt/gdt.hpp"
#include "x86_64/gdt/tss.hpp"

namespace firefly::kernel::core::gdt {
static GDT gdt_config;

void GDTconfig::set(int base, uint8_t flags, uint8_t access) {
    gdt_config.gdtd[base].base0 = 0;
    gdt_config.gdtd[base].base1 = 0;
    gdt_config.gdtd[base].base2 = 0;
    gdt_config.gdtd[base].limit = 0;
    gdt_config.gdtd[base].flags = flags;
    gdt_config.gdtd[base].access = access;

    // Don't register the NULL descriptor
    if (base != NULENT)
        gdt_config.registered_entries++;
}

void GDTconfig::set_tss(uint64_t base, uint8_t flags, uint8_t access) {
    gdt_config.tssd.size = 104;
    gdt_config.tssd.base0 = base & 0xFFFF;
    gdt_config.tssd.base1 = (base >> 16) & 0xFF;
    gdt_config.tssd.access = access;
    gdt_config.tssd.flags = flags;
    gdt_config.tssd.base2 = (base >> 24) & 0xFF;
    gdt_config.tssd.base3 = (base >> 32);
    gdt_config.tssd.reserved = 0;
}

uint16_t ltr_entry_offset() noexcept
{
    return gdt_config.registered_entries * 8;
}

uint16_t gdt_entry_offset(enum SELECTOR selector) noexcept
{
    return selector * 8;
}

void init() {
    GDTconfig().set(NULENT, 0, 0);
    GDTconfig().set(CS_KRN, 0x20, 0x9A);
    GDTconfig().set(DS_KRN, 0x20, 0x92);
    GDTconfig().set(CS_USR, 0x20, 0xFA);
    GDTconfig().set(DS_USR, 0x20, 0xF2);

    // Dummy TSS
    // Note that the tss is empty and should not be used until RSP0 is set
    // which is not the case until the memory manager is initialized
    GDTconfig().set_tss(0, 0x20, 0x89);

    // Can't call this in set_tss() since there
    // is only one tss per core. This is a dummy tss
    // so setting the actual tss later would change the offset in the GDT causing a GPF when loading the tr
    gdt_config.registered_entries++;

    gdtr_t gdtr;
    gdtr.size = (sizeof(GDT) + gdt_config.registered_entries) - 1;
    gdtr.base = reinterpret_cast<uint64_t>(&gdt_config);

    load_gdt((uint64_t)&gdtr);
    tss::load_tss(ltr_entry_offset());
}
}  // namespace firefly::kernel::core::gdt
