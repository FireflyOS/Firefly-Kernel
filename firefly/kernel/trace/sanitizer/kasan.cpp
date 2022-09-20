#include "firefly/trace/sanitizer/kasan.hpp"

#include <frg/init_once.hpp>

#include "cstdlib/cassert.h"
#include "firefly/compiler/compiler.hpp"
#include "firefly/drivers/ports.hpp"
#include "firefly/logger.hpp"
#include "firefly/memory-manager/virtual/virtual.hpp"
#include "firefly/panic.hpp"
#include "libk++/bits.h"
#include "libk++/cstring.hpp"
#include "libk++/memory.hpp"

/*
        Documentation is scarce, I found these resources to be helpful:
        - https://github.com/xairy/kernel-sanitizers/blob/asan/arch/x86/mm/asan/asan.c - This was an early test when kasan was just being integrated into the linux kernel.
        - https://www.starlab.io/blog/kasan-what-is-it-how-does-it-work-and-what-are-the-strange-numbers-at-the-end
*/

namespace {
using namespace firefly::kernel;

constexpr bool verbose = true;
constexpr uint64_t kasanShadowOffset = 0xdfff'f000'0000'0000;  // Turns into a canonical address when shifted calculating memory
constexpr uint8_t kasanShift = 3;
constexpr uint8_t kasanAccessSize = 1 << kasanShift;  // 8
constexpr uint32_t kasanHighShadowSize = (GiB(4) / 8);

bool kasanUserAddress(VirtualAddress ptr) {
    return (reinterpret_cast<uintptr_t>(ptr) & (1ul << 63)) == 0;
}

uint8_t *toShadow(VirtualAddress ptr) {
    return reinterpret_cast<uint8_t *>(
        (reinterpret_cast<uintptr_t>(ptr) >> kasanShift) + kasanShadowOffset);
}

[[maybe_unused]] VirtualAddress fromShadow(kasan::KasanAddress shadow) {
    return reinterpret_cast<VirtualAddress>((shadow >> kasanShift) + kasanShadowOffset);
}

// Q&D bounds-check (temporary).
bool withinKasanMemory(kasan::KasanAddress addr) {
    if (addr >= AddressLayout::High && addr <= AddressLayout::High + GiB(4))
        return true;

    return false;
}

void mapShadowMemory(VirtualAddress base, size_t length) {
    using namespace core::paging;

    auto shadow = uint64_t(toShadow(base));
    if constexpr (verbose) {
        info_logger << "KASAN: Mapping kasan shadow " << info_logger.hex(base) << " at " << info_logger.hex(shadow) << '\n';
    }

    // TODO: Is this the right way to go about this..?
    //       Managarm does this too when mapping kasan memory, it's just that this uses
    //		 so much memory...
    for (size_t i = 0; i < length; i += PAGE_SIZE) {
        auto phys_addr = uint64_t(mm::Physical::allocate());
        mm::kernelPageSpace::accessor().map(shadow + i, phys_addr, AccessFlags::ReadWrite);
    }
}
}  // namespace

namespace firefly::kernel::kasan {
frg::init_once<bool> kasan_initialized{ false };

[[gnu::no_sanitize_address]] inline bool isPoisoned(KasanAddress addr);

void init() {
    // NOTE: We're using AddressLayout::High as a test here. It should just shadow memory such as the heap or any buffers.
#if defined(FIREFLY_KASAN)
    mapShadowMemory(VirtualAddress(AddressLayout::High), kasanHighShadowSize);
    poison(VirtualAddress(AddressLayout::High));

    info_logger << "KASAN: Initialized\n";
    kasan_initialized = true;
#else
    info_logger << "KASAN is disabled\n";
#endif
}

[[gnu::no_sanitize_address]] void poison(VirtualAddress addr, ShadowRegionStatus status) {
#if defined(FIREFLY_KASAN)
    assert_truth(!kasanUserAddress(addr) && "Kasan cannot poison user pages yet!");
    memset(static_cast<void *>(toShadow(addr)), static_cast<uint8_t>(status), kasanHighShadowSize >> kasanShift);
#else
    (void)addr;
    (void)status;
#endif
}

[[gnu::no_sanitize_address]] void unpoison(VirtualAddress addr) {
#if defined(FIREFLY_KASAN)
    poison(addr, ShadowRegionStatus::Unpoisoned);
#else
    (void)addr;
#endif
}

[[gnu::no_sanitize_address]] inline bool isPoisoned(KasanAddress addr) {
    auto shadow = toShadow(VirtualAddress(addr));
    for (size_t i = 0; i < (kasanHighShadowSize >> kasanShift); i++) {
        if (shadow[i] == static_cast<uint8_t>(ShadowRegionStatus::Poisoned))
            return true;
    }

    return false;
}

[[gnu::no_sanitize_address]] void verifyAccess(KasanAddress addr, size_t size, bool write) {
    if (!kasan_initialized || !withinKasanMemory(addr))
        return;

    static int recursion;
    if (++recursion == 1) {
        if (isPoisoned(addr)) {
            info_logger << "KASAN: Memory access details:\n"
                        << (write ? "Write to " : "Read from ") << info_logger.hex(addr) << " of size " << size << '\n';
            panic("KASAN bug");
        }
    }
    recursion--;
}

}  // namespace firefly::kernel::kasan
