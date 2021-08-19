#include <stl/cstdlib/stdio.h>

#include <x86_64/gdt/gdt.hpp>

extern "C" void load_gdt(uint64_t);

namespace firefly::kernel::core::gdt {
static gdtd_t gdtd[GDT_MAX_ENTRIES];
void GDTconfig::set(int offset, uint8_t flags, uint8_t access) {
    gdtd[offset].base0 = 0;
    gdtd[offset].base1 = 0;
    gdtd[offset].base2 = 0;
    gdtd[offset].limit = 0;
    gdtd[offset].flags = flags;
    gdtd[offset].access = access;
}

void init() {
    GDTconfig().set(NULENT, 0, 0);
    GDTconfig().set(CS_KRN, 0x20, 0x9A);
    GDTconfig().set(DS_KRN, 0x20, 0x92);
    GDTconfig().set(CS_USR, 0x20, 0xFA);
    GDTconfig().set(DS_USR, 0x20, 0xF2);

    gdtr_t gdtr;
    gdtr.size = sizeof(gdtd_t) * GDT_MAX_ENTRIES - 1;
    gdtr.offset = reinterpret_cast<uint64_t>(gdtd);
    load_gdt((uint64_t)&gdtr);
}
}  // namespace firefly::kernel::core::gdt
