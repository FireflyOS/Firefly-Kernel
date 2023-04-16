#pragma once

#include <cstdint>

#include "firefly/compiler/compiler.hpp"

namespace firefly::kernel {
struct InterruptFrame {
    int64_t r15;
    int64_t r14;
    int64_t r13;
    int64_t r12;
    int64_t r11;
    int64_t r10;
    int64_t r9;
    int64_t r8;
    int64_t rdi;
    int64_t rsi;
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
} PACKED;

using ContextRegisters = InterruptFrame;

union RFlags {
    uint64_t All;
    struct {
        int8_t carry : 1;
        int8_t reserved0 : 1;
        int8_t parity : 1;
        int8_t reserved1 : 1;
        int8_t auxiliaryCarry : 1;
        int8_t reserved2 : 1;
        int8_t zero : 1;
        int8_t sign : 1;
        int8_t trap : 1;
        int8_t interruptEnable : 1;
        int8_t direction : 1;
        int8_t overflow : 1;
        int8_t ioPriv : 2;
        int8_t nesterTask : 1;
        int8_t reserved3 : 1;
        int8_t resume : 1;
        int8_t v8086Mode : 1;
        int8_t alignCheckAccessControl : 1;
        int8_t virtualInterrupt : 1;
        int8_t virtualInterruptPending : 1;
        int8_t id : 1;
        int64_t reserved4 : 41;
    } fields PACKED;
};

}  // namespace firefly::kernel
