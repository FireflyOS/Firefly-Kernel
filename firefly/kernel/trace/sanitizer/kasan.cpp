#include <atomic>

#include "firefly/drivers/ports.hpp"
#include "firefly/logger.hpp"
#include "firefly/memory-manager/virtual/virtual.hpp"
#include "firefly/panic.hpp"
#include "libk++/bits.h"

namespace firefly::kernel::kasan {
static bool kasan_initialized{ false };

[[gnu::no_sanitize_address]] void rip() {
    const char *j = "Uninitialized KASAN check!\n";

    for (int i = 0; i < 27; i++)
        firefly::kernel::io::outb(0xe9, j[i]);

    for (;;)
        ;
}

void init() {
    info_logger << "KASAN: Mapping kasan shadow at: " << info_logger.hex(AddressLayout::KasanShadow) << '\n';
    mm::kernelPageSpace::accessor().mapRange(AddressLayout::KasanShadow, GiB(1) /* TODO: Dynamically determine shadow memory size (It's 1/8th of the kernel's address space) */, firefly::kernel::mm::AccessFlags::ReadWrite);
    info_logger << "KASAN: Initialized\n";
    kasan_initialized = true;
}

[[gnu::no_sanitize_address]] void reportFailure(uintptr_t addr, size_t size, bool write) {
    // Hmm, so it looks like returning here breaks stuff?
    // Oh, you know it's probably because it returns right where the kasan check was called.
    // At least the debugger showed it was an infinite loop from __asan_load2 => reportFailure
    static int recursion;

    // if (!kasan_initialized)
    // rip();
    // return;

    if (++recursion == 1) {
        if (kasan_initialized)
		{
            info_logger << (write ? "Write to " : "Read from ") << info_logger.hex(addr) << " of size " << size << '\n';
    		panic("KASAN bug");
		}
        // for(;;)
        // 	;
    }
    recursion--;

    // Crashes, investigate...
}
}  // namespace firefly::kernel::kasan
