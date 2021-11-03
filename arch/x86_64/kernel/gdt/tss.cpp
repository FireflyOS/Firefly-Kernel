#include "x86_64/gdt/gdt.hpp"

namespace firefly::kernel::core::tss {

void core0_tss_init(size_t base) {
    tss_t tss_core0 = *reinterpret_cast<tss_t*>(&base);
    gdt::GDTconfig().set_tss(reinterpret_cast<uint64_t>(&tss_core0), 0x20, 0x89);
    load_tss(gdt::ltr_entry_offset());
}

void load_tss(uint16_t gdt_offset)
{
    asm volatile(
        "ltr %%ax\n"
        :: "a"(gdt_offset)
    );
}

}  // namespace firefly::kernel::core::tss