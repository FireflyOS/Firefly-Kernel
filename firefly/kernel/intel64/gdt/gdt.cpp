#include "firefly/intel64/gdt/gdt.hpp"

#include "firefly/intel64/gdt/tss.hpp"

namespace firefly::kernel::core::gdt {
static GDT gdt_config;

void GDTconfig::set(int base, uint8_t flags, uint8_t access, uint16_t limit) {
    gdt_config.gdtd[base].base0 = 0;
    gdt_config.gdtd[base].base1 = 0;
    gdt_config.gdtd[base].base2 = 0;
    gdt_config.gdtd[base].limit = limit;
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

uint16_t ltr_entry_offset() noexcept {
    return gdt_config.registered_entries * 8;
}

uint16_t gdt_entry_offset(enum SELECTOR selector) noexcept {
    return selector * 8;
}

void init() {
    GDTconfig().set(NULENT, 0, 0);
    GDTconfig().set(CS_KRN16, 0x80, 0x9A, 0xFFFF);
    GDTconfig().set(DS_KRN16, 0x80, 0x9A, 0xFFFF);
    GDTconfig().set(CS_KRN32, 0xCF, 0x9A, 0xFFFF);
    GDTconfig().set(DS_KRN32, 0xCF, 0x92, 0xFFFF);
    GDTconfig().set(CS_KRN64, 0xA2, 0x9A);
    GDTconfig().set(DS_KRN64, 0xA0, 0x92);
    GDTconfig().set(DS_USR64, 0x00, 0xF2);
    GDTconfig().set(CS_USR64, 0x20, 0xFA);

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
