#include "firefly/intel64/int/interrupt.hpp"

#include <cstdint>

#include "firefly/drivers/ports.hpp"
#include "firefly/intel64/cpu/apic/apic.hpp"
#include "firefly/logger.hpp"
#include "firefly/panic.hpp"
#include "firefly/trace/symbols.hpp"

namespace firefly::kernel::core::interrupt {
struct __attribute__((packed)) idt_gate {
    uint16_t offset_0;
    uint16_t selector;
    uint8_t rsv_0;
    uint8_t type;
    uint16_t offset_1;
    uint32_t offset_2;
    uint32_t rsv_1;
};

static_assert(16 == sizeof(idt_gate), "idt_gate size incorrect");

struct __attribute__((packed)) iframe {
    int64_t r15;
    int64_t r14;
    int64_t r13;
    int64_t r12;
    int64_t r11;
    int64_t r10;
    int64_t r9;
    int64_t r8;
    int64_t rsi;
    int64_t rdi;
    int64_t rbp;
    int64_t rdx;
    int64_t rcx;
    int64_t rbx;
    int64_t rax;
    int64_t int_no;
    int64_t err;
    int64_t rip;
    int64_t cs;
    int64_t rflags;
    int64_t rsp;
    int64_t ss;
};

extern "C" {
void interrupt_handler(iframe iframe);
void irq_handler(iframe iframe);
void exception_handler([[maybe_unused]] iframe iframe);
void interrupt_wrapper();
void exception_wrapper();
void assign_cpu_exceptions();
void assign_irq_handlers();
}

static idt_gate idt[256];

static const char* exceptions[] = {
    "Divide by Zero",
    "Debug",
    "NMI",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device not available",
    "Double Fault",
    "",
    "Invalid TSS",
    "Segment not present",
    "Stack Segment fault",
    "GPF",
    "Page Fault",
    "",
    "x87 Floating point exception",
    "Alignment check",
    "Machine check",
    "SIMD Floating point exception",
    "Virtualization Exception",
    "Control protection exception",
    "",
    "",
    "",
    "",
    "",
    "",
    "Hypervisor injection exception",
    "VMM Communication exception",
    "Security exception",
    ""
};

namespace change {
extern "C" void update(void (*handler)(), uint16_t cs, uint8_t type, uint8_t index) {
    idt[index].offset_0 = reinterpret_cast<size_t>(handler) & 0xffff;
    idt[index].selector = cs;
    idt[index].rsv_0 = 0;
    idt[index].type = type;
    idt[index].offset_1 = reinterpret_cast<size_t>(handler) >> 16 & 0xffff;
    idt[index].offset_2 = reinterpret_cast<size_t>(handler) >> 32 & 0xffffffff;
    idt[index].rsv_1 = 0;
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
    idt_gate* base;
} idtr = {
    .limit = (sizeof(struct idt_gate) * 256) - 1,
    .base = idt
};

void init() {
    assign_cpu_exceptions();
    assign_irq_handlers();

    asm("lidt %0" ::"m"(idtr)
        : "memory");
}

void interrupt_handler(iframe iframe) {
    ConsoleLogger::log() << "Exception: " << exceptions[iframe.int_no] << logger::endl;  // this should be safe as the interrupt handler may only be called by int 0x0-0x1F
    ConsoleLogger::log() << "Int#: " << iframe.int_no << logger::endl;
    ConsoleLogger::log() << "Error code: " << iframe.err << logger::endl;
    ConsoleLogger::log() << "RIP: " << ConsoleLogger::log().hex(iframe.rip) << logger::endl;
    SerialLogger::log() << "RIP: " << SerialLogger::log().hex(iframe.rip) << logger::endl;

    SerialLogger() << "Rax: " << SerialLogger::log().hex(iframe.rax) << '\n'
                   << "Rbx: " << SerialLogger::log().hex(iframe.rbx) << '\n'
                   << "Rcx: " << SerialLogger::log().hex(iframe.rcx) << '\n'
                   << "Rdx: " << SerialLogger::log().hex(iframe.rdx) << '\n'
                   << "Rdi: " << SerialLogger::log().hex(iframe.rdi) << '\n'
                   << "Rsi: " << SerialLogger::log().hex(iframe.rsi) << '\n'
                   << "R8: " << SerialLogger::log().hex(iframe.r8) << '\n'
                   << "R9: " << SerialLogger::log().hex(iframe.r9) << '\n'
                   << "R10: " << SerialLogger::log().hex(iframe.r10) << '\n'
                   << "R11: " << SerialLogger::log().hex(iframe.r11) << '\n'
                   << "R12: " << SerialLogger::log().hex(iframe.r12) << '\n'
                   << "R13: " << SerialLogger::log().hex(iframe.r13) << '\n'
                   << "R14: " << SerialLogger::log().hex(iframe.r14) << '\n'
                   << "R15: " << SerialLogger::log().hex(iframe.r15) << '\n';

    for (;;)
        asm("cli\nhlt");
}

// TODO: maybe use a frg style array?
static void (*irqHandlers[24])() = { 0 };

void registerIRQHandler(void (*handler)(), uint8_t irq) {
    irqHandlers[irq] = handler;
}

void irq_handler(iframe iframe) {
    uint8_t irq = iframe.int_no - apic::LVT_BASE;
    if (irqHandlers[irq] != nullptr) {
        irqHandlers[irq]();
    } else {
        SerialLogger::log() << "Unhandled IRQ received #" << iframe.int_no - apic::LVT_BASE << "\n";
    }
    apic::Apic::accessor().sendEOI();
}

}  // namespace firefly::kernel::core::interrupt
