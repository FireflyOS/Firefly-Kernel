#include "i386/gdt/gdt.hpp"

namespace firefly::kernel::core::gdt {

segment_descriptor_t generate_descriptor(uint32_t base, size_t limit, bool executable) {
    segment_descriptor_t ret;
    // zero it out
    *reinterpret_cast<uint64_t*>(&ret) = 0;

    // set the base and limit
    // 16 bits
    ret.base_low = base & 0xFFFF;
    // 8 bits 
    ret.base_middle = (base >> 16) & 0xFF;
    // 8 bits
    ret.base_high = (base >> 24) & 0xFF;

    // 16 bits
    ret.limit_low = limit & 0xFFFF;
    // 4 bits 
    ret.limit_high = (limit >> 16) & 0xF0;

    // AC RW DC Ex S PRIVL PR
    // 0  1  0  1  1 00    1  0x9A
    // 0  1  0  0  1 00    1  0x92
    // 0b1001 1010 0x9A
    // 0b1001 0010 0x92
    // This is for code
    ret.access.accessed = 0;
    ret.access.read_writable = 1;
    ret.access.direction = 0;
    ret.access.executable = executable;
    ret.access.type = 1;
    ret.access.privilege = 0;
    ret.access.present = 1;

    return ret;    
}

static segment_descriptor_t gdt[GDT_ENTRIES];
struct __attribute__((packed)) gdt_descriptor_t {
    uint16_t size;
    uint32_t offset;
};

static gdt_descriptor_t gdt_descriptor;

void init() {
    gdt[0] = generate_descriptor(0, 0, 0);
    // Set all the flags to 0 for the null-segment
    *reinterpret_cast<uint8_t*>(&gdt[0].access) = 0;

    // Kernel code segment
    gdt[1] = generate_descriptor(0, 0xFFFFFFFF, 1);
    // Kernel data segment
    gdt[2] = generate_descriptor(0, 0xFFFFFFFF, 0);
    gdt_descriptor.size = GDT_ENTRIES - 1;
    gdt_descriptor.offset = reinterpret_cast<uint32_t>(gdt);


    asm volatile("lgdt %[gdt_descriptor]" : : [gdt_descriptor] "m" (gdt_descriptor));
}

}  // namespace firefly::kernel::core::gdt