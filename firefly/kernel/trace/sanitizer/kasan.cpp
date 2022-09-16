#include <atomic>

#include "firefly/drivers/ports.hpp"
#include "firefly/logger.hpp"
#include "firefly/memory-manager/virtual/virtual.hpp"
#include "firefly/panic.hpp"
#include "libk++/bits.h"

namespace firefly::kernel::kasan {
std::atomic_bool kasan_initialized{ false };

void init() {
    info_logger << "KASAN: Mapping kasan shadow at: " << info_logger.hex(AddressLayout::KasanShadow) << '\n';
    mm::kernelPageSpace::accessor().mapRange(AddressLayout::KasanShadow, GiB(1) /* TODO: Dynamically determine shadow memory size (It's 1/8th of the kernel's address space) */, firefly::kernel::mm::AccessFlags::ReadWrite);
    info_logger << "KASAN: Initialized\n";
    kasan_initialized = true;
}

[[gnu::no_sanitize_address]] void reportFailure(uintptr_t addr, size_t size, bool write) {
    static int recursion;

    if (++recursion == 1) {
        if (kasan_initialized) {
            info_logger << (write ? "Write to " : "Read from ") << info_logger.hex(addr) << " of size " << size << '\n';
            panic("KASAN bug");
        }
    }
    recursion--;
}
}  // namespace firefly::kernel::kasan
