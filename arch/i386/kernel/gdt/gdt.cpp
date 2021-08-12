#include "i386/gdt/gdt.hpp"

namespace firefly::kernel::core::gdt {

static segment_descriptor_t gdt[GDT_ENTRIES];

void generate_descriptor(int index, uint32_t base, size_t limit, uint8_t access, uint8_t gran) {
    *reinterpret_cast<uint32_t*>(&gdt) = 0;
    gdt[index].base_low = base & 0xFFFF;
    gdt[index].base_middle = (base >> 16) & 0xFF;
    gdt[index].base_high = (base >> 24) & 0xFF;
    gdt[index].limit_low = (limit & 0xFFFF);
    gdt[index].granularity = (limit >> 16) & 0x0F;
    gdt[index].granularity |= gran & 0xF0;
    gdt[index].access = access;
}

struct __attribute__((packed)) gdt_descriptor_t {
    uint16_t size;
    uint32_t offset;
};

gdt_descriptor_t gdt_descriptor;

extern "C" void gdt_flush(uint32_t);
void init() {
    //Null segment
    generate_descriptor(0, 0, 0, 0, 0);

    // Kernel code segment
    generate_descriptor(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

    // Kernel data segment
    generate_descriptor(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

    // User code segment
    generate_descriptor(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);

    // User data segment
    generate_descriptor(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);


    gdt_descriptor.size = sizeof(struct gdt_descriptor_t) * GDT_ENTRIES - 1;
    gdt_descriptor.offset = reinterpret_cast<uint32_t>(&gdt);
    gdt_flush((uint32_t)&gdt_descriptor);
}

}  // namespace firefly::kernel::core::gdt