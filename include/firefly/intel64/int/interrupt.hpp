#pragma once

#include <cstdint>

#include "firefly/scheduler/scheduler.hpp"

namespace firefly::kernel::core::interrupt {
void init();

struct __attribute__((packed)) GeneralPurposeRegs {
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
};

struct __attribute__((packed)) iframe {
    GeneralPurposeRegs gpr;
    int64_t int_no;
    int64_t err;
    int64_t rip;
    int64_t cs;
    int64_t rflags;
    int64_t rsp;
    int64_t ss;
};

// register IRQ handler
// No need to call update() as we just write it to a table with handlers
// for the actual interrupt handler
void registerIRQHandler(void (*handler)(uint8_t int_num, scheduler::RegisterContext* regs), uint8_t irq);
void unregisterIRQHandler(uint8_t irq);

namespace change {
extern "C" void update(void (*handler)(uint8_t int_num, scheduler::RegisterContext* regs), uint16_t cs, uint8_t type, uint8_t index);
}
}  // namespace firefly::kernel::core::interrupt
