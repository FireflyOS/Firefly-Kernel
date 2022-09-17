#pragma once

#include "firefly/intel64/gdt/gdt.hpp"
#include "firefly/intel64/gdt/tss.hpp"
#include <libk++/bits.h>

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
static inline void native_cpuid(unsigned int *eax, unsigned int *ebx,
                                unsigned int *ecx, unsigned int *edx)
{
        /* ecx is often an input as well as an output. */
        asm volatile("cpuid"
            : "=a" (*eax),
              "=b" (*ebx),
              "=c" (*ecx),
              "=d" (*edx)
            : "0" (*eax), "2" (*ecx));
}

static inline bool cpu_support_onegb_pages() {
	unsigned eax, ebx, ecx, edx;
	eax = 1;
	native_cpuid(&eax, &ebx, &ecx, &edx);

	// check if bit 26 of edx is set
	return (edx & BIT(26));
}

}  // namespace firefly::kernel
