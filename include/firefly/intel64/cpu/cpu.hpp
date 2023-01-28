#pragma once

#include "firefly/intel64/gdt/gdt.hpp"
#include "firefly/intel64/gdt/tss.hpp"
#include "libk++/bits.h"

namespace firefly::kernel {
struct __attribute__((packed)) InterruptFrame {
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
};

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

enum MSR : uint32_t {
    GsBase = 0xC0000101,
};

// Per-cpu structures
struct AssemblyCpuData {
    AssemblyCpuData *selfPointer;
} PACKED;

struct CpuData : public AssemblyCpuData {
    uint8_t cpuIndex;

    core::gdt::Gdt gdt;
    core::tss::Tss tss;
} PACKED;

// from managarm
void setupCpuContext(AssemblyCpuData *context);
void setupBootCpuContext();

void initializeBootProccessor(uint64_t stack);
void initializeApplicationProcessor(uint64_t stack);

void initializeThisCpu(uint64_t stack);
CpuData *getCpuData(size_t k);
CpuData *getCpuData();

// from linux kernel source
inline void native_cpuid(uint32_t *eax, uint32_t *ebx,
                         uint32_t *ecx, uint32_t *edx) {
    /* ecx is often an input as well as an output. */
    asm volatile("cpuid"
                 : "=a"(*eax),
                   "=b"(*ebx),
                   "=c"(*ecx),
                   "=d"(*edx)
                 : "0"(*eax), "2"(*ecx));
}

inline uint64_t rdtsc(void) {
    uint32_t edx, eax;
    asm volatile("rdtsc"
                 : "=a"(eax), "=d"(edx));
    return ((uint64_t)edx << 32) | eax;
}

inline bool cpuHugePages() {
    uint32_t eax{ 1 }, ebx{ 0 }, ecx{ 0 }, edx{ 0 };
    native_cpuid(&eax, &ebx, &ecx, &edx);

    // check if bit 26 of edx is set
    return (edx & BIT(26));
}

inline void delay(uint64_t cycles) {
    uint64_t next_stop = rdtsc() + cycles;
    while (rdtsc() < next_stop)
        ;
}

inline void wrmsr(uint32_t index, uint64_t value) {
    uint32_t low = value;
    uint32_t high = value >> 32;
    asm volatile("wrmsr" ::"c"(index), "a"(low), "d"(high)
                 : "memory");
}

inline uint64_t rdmsr(uint32_t index) {
    uint32_t low, high;
    asm volatile("rdmsr"
                 : "=a"(low), "=d"(high)
                 : "c"(index)
                 : "memory");
    return ((uint64_t)high << 32) | (uint64_t)low;
}

}  // namespace firefly::kernel
