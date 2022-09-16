#include <atomic>

#include "firefly/logger.hpp"
#include "firefly/memory-manager/virtual/virtual.hpp"
#include "libk++/bits.h"

namespace firefly::kernel::kasan {
static bool kasan_initialized{ false };

void init() {
    info_logger << "KASAN: Mapping kasan shadow at: " << info_logger.hex(AddressLayout::KasanShadow) << '\n';
    mm::kernelPageSpace::accessor().mapRange(AddressLayout::KasanShadow, GiB(1) /* TODO: Dynamically determine shadow memory size (It's 1/8th of the kernel's address space) */, firefly::kernel::mm::AccessFlags::ReadWrite);
    info_logger << "KASAN: Initialized\n";
    kasan_initialized = true;
}

void reportFailure(uintptr_t addr, size_t size, bool write) {
    if (!kasan_initialized)
        return;

    info_logger << (write ? "Write to " : "Read from ") << info_logger.hex(addr) << " of size " << size << '\n';
	for(;;)
		;

	// Crashes, investigate...
    // firefly::panic("KASAN bug");
}
}  // namespace firefly::kernel::kasan
