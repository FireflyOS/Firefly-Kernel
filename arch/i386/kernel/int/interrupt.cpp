#include <i386/libk++/ios.h>
#include <i386/libk++/iostream.h>

#include <i386/drivers/vga.hpp>

using namespace firefly::drivers::vga;

namespace firefly::kernel::core::interrupt {
struct __attribute__((packed)) idt_gate {
    uint16_t offset_0;
    uint16_t selector;
    uint8_t rsv_0;
    uint8_t type;
    uint16_t offset_1;
};
static_assert(8 == sizeof(idt_gate), "idt_gate size incorrect");

struct __attribute__((packed)) iframe {
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t esp;
    uint32_t ss;
    uint32_t err;
    uint32_t int_no;
};

__attribute__((interrupt)) __attribute__((noreturn)) void interrupt_wrapper([[maybe_unused]] iframe *iframe);
__attribute__((interrupt)) __attribute__((noreturn)) void exception_wrapper([[maybe_unused]] iframe *iframe);

idt_gate idt[256] = {};
namespace change {
void update(uint32_t handler, uint16_t cs, uint8_t type, uint8_t index) {
    idt[index].offset_0 = handler & 0xffff;
    idt[index].selector = cs;
    idt[index].rsv_0 = 0;
    idt[index].type = type;
    idt[index].offset_1 = handler >> 16 & 0xffff;
}

//Hardcoded values as this is only meant for initialisation work by interrupt.cpp
//Use the non-static version of "update" to update the idt at a global level
static void initial_update(uint32_t handler, uint8_t index) {
    change::update(handler, 0x10 /* CHANGE ME */, 0x8E, index);
}

}  // namespace change

struct __attribute__((packed)) idt_reg {
    /**
         *                  size of table in bytes - 1
         */
    uint16_t limit;
    /**
         *                  base address of idt
         */
    uint32_t base;
} idtr = {
    .limit = (sizeof(struct idt_gate) * 256) - 1,
    .base = (uint32_t)idt
};


void init() {
    int i = 0;
    for (; i <= 31; i++)
        change::initial_update(reinterpret_cast<uint32_t>(interrupt_wrapper), i);
    for (; i < 256; i++)
        change::initial_update(reinterpret_cast<uint32_t>(exception_wrapper), i);

    asm("lidt %0" ::"m"(idtr)
        : "memory");
}

void test_int() {
    klog("testing interrupt 0...\n");
    asm volatile("int $0");
}

__attribute__((interrupt)) __attribute__((noreturn)) void interrupt_wrapper([[maybe_unused]] iframe *iframe) {
    char buff[20];
    klog("CPU exception caught (CS: 0x" << itoa(iframe->cs, buff, 16) << ")\n");

    for (;;)
        asm("cli;hlt");
}

__attribute__((interrupt)) __attribute__((noreturn)) void exception_wrapper([[maybe_unused]] iframe *iframe) {
    // klog("An Interrupt has occurred: " << _iframe->cs);

    for (;;)
        asm("cli;hlt");
}
}  // namespace firefly::kernel::core::interrupt