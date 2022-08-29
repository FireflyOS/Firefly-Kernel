#include "firefly/intel64/gdt/gdt.hpp"

#include "firefly/intel64/gdt/tss.hpp"

namespace firefly::kernel::core::gdt {

void setGdtEntry(Gdt &cpuGdt, uint64_t selectorIdx, uint8_t flags, uint8_t access) {
    cpuGdt.gdtDescriptors[selectorIdx].flags = flags;
    cpuGdt.gdtDescriptors[selectorIdx].access = access;
}

void setTssEntry(Gdt &cpuGdt, uint64_t selectorIdx, uint8_t flags, uint8_t access) {
    cpuGdt.tssDescriptors.size = 104;
    cpuGdt.tssDescriptors.base0 = selectorIdx & 0xFFFF;
    cpuGdt.tssDescriptors.base1 = (selectorIdx >> 16) & 0xFF;
    cpuGdt.tssDescriptors.access = access;
    cpuGdt.tssDescriptors.flags = flags;
    cpuGdt.tssDescriptors.base2 = (selectorIdx >> 24) & 0xFF;
    cpuGdt.tssDescriptors.base3 = (selectorIdx >> 32);
    cpuGdt.tssDescriptors.reserved = 0;
}

uint16_t tssEntryOffset() {
    return GDT_MAX_ENTRIES * 8;
}

uint16_t gdtEntryOffset(enum SegmentSelector selector) {
    return selector * 8;
}

void init(Gdt &gdt) {
    setGdtEntry(gdt, SegmentSelector::null, 0, 0);
    setGdtEntry(gdt, SegmentSelector::kernCS, 0xA2, 0x9A);
    setGdtEntry(gdt, SegmentSelector::kernDS, 0xA0, 0x92);
    setGdtEntry(gdt, SegmentSelector::userDS, 0x00, 0xF2);
    setGdtEntry(gdt, SegmentSelector::userCS, 0x20, 0xFA);

    Gdtr gdtr;
    gdtr.size = (sizeof(Gdt) + GDT_MAX_ENTRIES) - 1;
    gdtr.base = reinterpret_cast<uint64_t>(&gdt);

    load_gdt((uint64_t)&gdtr);
}
}  // namespace firefly::kernel::core::gdt
