#pragma once

#include "firefly/intel64/gdt/gdt.hpp"
#include "firefly/intel64/gdt/tss.hpp"
#include "libk++/bits.h"

// Note: This is purposely bare-bones and lacking a
// proper "per cpu" structure (i.e. gsbase and co).
// There is no smp yet, but I'd prefer to structure
// everything in a way so that it's easy to get it up and running.
namespace firefly::kernel {

// Per-cpu structure
struct CpuData {
    core::gdt::Gdt gdt;
    core::tss::Tss tss;
} PACKED;

void initializeThisCpu(uint64_t stack);
CpuData &thisCpu();

// from linux kernel source
static inline void native_cpuid(uint32_t *eax, uint32_t *ebx,
                                uint32_t *ecx, uint32_t *edx)
{
        /* ecx is often an input as well as an output. */
        asm volatile("cpuid"
            : "=a" (*eax),
              "=b" (*ebx),
              "=c" (*ecx),
              "=d" (*edx)
            : "0" (*eax), "2" (*ecx));
}

inline uint64_t rdtsc(void) {
    uint32_t edx, eax;
    asm volatile ("rdtsc" : "=a" (eax), "=d" (edx));
    return ((uint64_t)edx << 32) | eax;
}

static inline bool cpuHugePages() {
	uint32_t eax{1}, ebx{0}, ecx{0}, edx{0};
	native_cpuid(&eax, &ebx, &ecx, &edx);

	// check if bit 26 of edx is set
	return (edx & BIT(26));
}

static inline void delay(uint64_t cycles) {
	uint64_t next_stop = rdtsc() + cycles;
	while (rdtsc() < next_stop);
}

}  // namespace firefly::kernel
