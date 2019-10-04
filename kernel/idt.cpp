#include <stl/cstdlib/cstdint.h>
#include <drivers/vga.hpp>
#include <stl/array.h>

// type doesn't really exist in C++
// so just decl as function so it decays into a pointer
extern "C" void interrupt_wrapper();
extern "C" void exception_wrapper();

struct __attribute__((packed)) idt_gate {
    /**
     *                      bits 15 : 0 of offset
     */
    uint16_t offset_0;

    /**
     *                      code segment selector in GDT or LDT
     */
    uint16_t selector;

    /**
     *                      zero
     */
    uint8_t rsv_0;

    /**
     *                      interrupt gate type
     */
    enum gate_type {
        GATE_TASK = 0x5,
        GATE_CALL = 0xC,
        GATE_INTERRUPT = 0xE,
        GATE_TRAP = 0xF,
    } type : 5;

    /**
     *                      a
     */
    unsigned desc_priv_lvl : 2;

    /**
     *                      a
     */
    unsigned present : 1;

    /**
     *                      bits 31 :16 of offset
     */
    uint16_t offset_1;

    /**
     *                      bits 63 : 32 of offset
     */
    uint32_t offset_2;

    /**
     *                      zero
     */
    uint32_t rsv_1;
};

static_assert(16 == sizeof(idt_gate), "idt_gate size incorrect");

struct __attribute__((packed)) interrupt_error {
    unsigned ext : 1;
    unsigned idt : 1;
    unsigned gdt : 1;
    unsigned selector : 13;
    unsigned rsv_0 : 16;
    unsigned rsv_1;
};

static_assert(8 == sizeof(interrupt_error), "interrupt_error size incorrect");

// will probably not use, but keeping in case we want it
// struct __attribute__((packed)) interrupt_frame {
//     uint64_t rip;
//     uint64_t cs;
//     uint64_t rflags;
//     uint64_t rsp;
//     uint64_t ss;
// }

// static_assert(8 == sizeof(interrupt_frame, "interrupt_frame size incorrect"));

static firefly::std::array<idt_gate, 255> idt {
    // division interrupt
    { static_cast<uint16_t>(reinterpret_cast<uint64_t>(interrupt_wrapper)), 8, 0, idt_gate::GATE_INTERRUPT, 0, 1,
      static_cast<uint16_t>(reinterpret_cast<uint64_t>(interrupt_wrapper) >> 16),
      static_cast<uint32_t>(reinterpret_cast<uint64_t>(interrupt_wrapper) >> 32), 0 }
    
    // all others have present flag set to 0
};

struct __attribute__((packed)) idt_reg {
    uint16_t limit;
    idt_gate* base;
} idtr = {
    255,
    idt.begin()
};

static_assert(10 == sizeof(idt_reg), "idt_reg size incorrect");

void init_idt() {
    asm(
        "lidt (%0)"
        :
        : "r"(idt.begin())
    );
}

void div_zero() {
    Display x{};
    x << "DIVIVING BY ZERO";
    asm volatile("int3");
}

// write different handlers for each irpt + exc later
// noreturn for testing purposes, will remove later
extern "C" __attribute__((noreturn))
void interrupt_handler() {
    Display x{};
    x << "INTERRUPT OCCURRED";

    while(1);
}

extern "C" __attribute__((noreturn))
void exception_handler(interrupt_error error_code) {
    (void) error_code;

    Display x{};
    x << "EXCEPTION OCCURRED";

    while(1);
}