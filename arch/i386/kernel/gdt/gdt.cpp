#include "i386/gdt/gdt.hpp"

namespace firefly::kernel::core::gdt {

segment_descriptor_t gdt[GDT_ENTRIES];

void generate_descriptor(int index, uint32_t base, size_t limit, uint8_t access, uint8_t gran) {
    // zero it out
    *reinterpret_cast<uint32_t*>(&gdt) = 0;
    gdt[index].base_low = base & 0xFFFF;
    gdt[index].base_middle = (base >> 16) & 0xFF;
    gdt[index].base_high = (base >> 24) & 0xFF;
    gdt[index].limit_low = (limit & 0xFFFF);
    gdt[index].granularity = (limit >> 16) & 0x0F;
    gdt[index].granularity |= gran & 0xF0;
    gdt[index].access = access;

    // // set the base and limit
    // // 16 bits
    // ret.base_low = base & 0xFFFF;
    // // 8 bits
    // ret.base_middle = (base >> 16) & 0xFF;
    // // 8 bits
    // ret.base_high = (base >> 24) & 0xFF;

    // // 16 bits
    // ret.limit_low = limit & 0xFFFF;

    // // AC RW DC Ex S PRIVL PR
    // // 0  1  0  1  1 00    1  0x9A
    // // 0  1  0  0  1 00    1  0x92
    // // 0b1001 1010 0x9A
    // // 0b1001 0010 0x92
    // // This is for code
    // // ret.access = access;
    // ret.access.accessed = 0;
    // ret.access.read_writable = 1;
    // ret.access.direction = 0;
    // ret.access.executable = executable;
    // ret.access.type = 1;
    // ret.access.privilege = 0;
    // ret.access.present = 1;

    // ret.granularity = (limit >> 16) & 0x0F;
    // ret.granularity |= gran & 0xF0;

    // return ret;
}

struct __attribute__((packed)) gdt_descriptor_t {
    uint16_t size;
    uint32_t offset;
};

gdt_descriptor_t gdt_descriptor;

extern "C" void gdt_flush(uint32_t);
void init() {
    generate_descriptor(0, 0, 0, 0, 0);
    // Set all the flags to 0 for the null-segment
    *reinterpret_cast<uint8_t*>(&gdt[0].access) = 0;

    // Kernel code segment
    generate_descriptor(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

    // Kernel data segment
    generate_descriptor(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

    // User code segment
    generate_descriptor(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);

    // User data segment
    generate_descriptor(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);


    gdt_descriptor.size = sizeof(gdt_descriptor_t) * GDT_ENTRIES - 1;
    gdt_descriptor.offset = reinterpret_cast<uint32_t>(&gdt);
    gdt_flush((uint32_t)&gdt_descriptor);
}

}  // namespace firefly::kernel::core::gdt