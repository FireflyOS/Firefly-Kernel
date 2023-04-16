#pragma once

#include "firefly/intel64/gdt/gdt.hpp"
#include "firefly/intel64/gdt/tss.hpp"
#include "firefly/tasks/scheduler/scheduler.hpp"
#include "libk++/bits.h"

namespace firefly::kernel {

enum MSR : uint32_t {
    GsBase = 0xC0000101,
};

// Per-cpu structures
struct AssemblyCpuData {
    AssemblyCpuData *selfPointer;
};

struct CpuData : public AssemblyCpuData {
    uint8_t cpuIndex;

    core::gdt::Gdt gdt;
    core::tss::Tss tss;
    kernel::tasks::Scheduler *scheduler;
};

// from managarm
void initializeBootProccessor(uint64_t stack);
void initializeApplicationProcessor(uint64_t stack);

void initializeThisCpu(uint64_t stack);
CpuData *getCpuData(size_t k);
CpuData *getLocalCpuData();

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
