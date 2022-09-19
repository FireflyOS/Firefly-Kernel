#include "firefly/trace/sanitizer/kasan.hpp"

#include <frg/init_once.hpp>

#include "firefly/compiler/compiler.hpp"
#include "firefly/drivers/ports.hpp"
#include "firefly/logger.hpp"
#include "firefly/memory-manager/virtual/virtual.hpp"
#include "firefly/panic.hpp"
#include "libk++/bits.h"

namespace firefly::kernel::kasan {
frg::init_once<bool> kasan_initialized{ false };

void init() {
    info_logger << "KASAN: Mapping kasan shadow at: " << info_logger.hex(AddressLayout::KasanShadow) << '\n';
    mm::kernelPageSpace::accessor().mapRange(AddressLayout::KasanShadow, GiB(1) /* TODO: Dynamically determine shadow memory size (It's 1/8th the size of the heap (we don't have that yet)) */, firefly::kernel::mm::AccessFlags::ReadWrite);
    info_logger << "KASAN: Initialized\n";
    kasan_initialized = true;
}

[[gnu::no_sanitize_address]] void poison(KasanAddress addr) {
    (void)addr;
}

[[gnu::no_sanitize_address]] void unpoison(KasanAddress addr) {
    (void)addr;
}

[[gnu::no_sanitize_address]] inline bool isPoisoned(KasanAddress addr) {
    info_logger << "Checking if " << info_logger.hex(addr) << " is poisoned\n";
    return true;
}

[[gnu::no_sanitize_address]] void verifyAccess(KasanAddress addr, size_t size, bool write) {
    if (!kasan_initialized)
        return;

    static int recursion;
    if (++recursion == 1) {
        if (isPoisoned(addr)) {
            info_logger << (write ? "Write to " : "Read from ") << info_logger.hex(addr) << " of size " << size << '\n';
            panic("KASAN bug");
        }
    }
    recursion--;
}

}  // namespace firefly::kernel::kasan
