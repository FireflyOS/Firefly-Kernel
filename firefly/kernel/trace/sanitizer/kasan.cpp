#include "firefly/trace/sanitizer/kasan.hpp"

#include <frg/formatting.hpp>
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

constexpr bool verboseKasan = true;
constexpr bool debugKasan = false;
constexpr uint8_t kasanDumpSize = 16;                          // Dump 16 bits / 2 bytes at a time.
constexpr uint64_t kasanShadowOffset = 0xdfff'f000'0000'0000;  // Turns into a canonical address in {to,from}Shadow
constexpr uint8_t kasanShift = 3;
constexpr uint8_t kasanAccessSize = 1 << kasanShift;    // 8
constexpr uint32_t kasanHighShadowSize = (GiB(4) / 8);  // Temporary. Should be the size of the kernel heap (which does not exist yet) divided by 8.

#if !defined(FIRELY_KASAN)
[[maybe_unused]]
#endif
bool
kasanUserAddress(VirtualAddress ptr) {
    return (reinterpret_cast<uintptr_t>(ptr) & (1ul << 63)) == 0;
}

uint8_t *toShadow(VirtualAddress ptr) {
    return reinterpret_cast<uint8_t *>(
        (reinterpret_cast<uintptr_t>(ptr) >> kasanShift) + kasanShadowOffset);
}

VirtualAddress fromShadow(kasan::KasanAddress shadow) {
    return reinterpret_cast<VirtualAddress>((shadow - kasanShadowOffset) << kasanShift);
}

// Q&D bounds-check (temporary, should be the kernel heap).
bool withinKasanMemory(kasan::KasanAddress addr) {
    if (addr >= AddressLayout::High && addr <= AddressLayout::High + GiB(4))
        return true;

    return false;
}

#if !defined(FIRELY_KASAN)
[[maybe_unused]]
#endif
void
mapShadowMemory(VirtualAddress base, size_t length) {
    using namespace core::paging;

    auto shadow = uint64_t(toShadow(base));
    if constexpr (verboseKasan) {
        firefly::logLine << "KASAN: Mapping kasan shadow " << firefly::fmt::hex << reinterpret_cast<uintptr_t>(base) << " at " << firefly::fmt::hex << shadow << '\n'
                         << firefly::fmt::endl;
    }

    // TODO: Is this the right way to go about this..?
    //       Managarm does this too when mapping kasan memory, it's just that this uses
    //		 so much memory...
    for (size_t i = 0; i <= length; i += PageSize::Size4K) {
        auto phys_addr = uint64_t(mm::Physical::allocate());
        mm::kernelPageSpace::accessor().map(shadow + i, phys_addr, AccessFlags::ReadWrite, PageSize::Size4K);
    }
}
}  // namespace

namespace firefly::kernel::kasan {
frg::init_once<bool> kasan_initialized{ false };

void init() {
#if defined(FIREFLY_KASAN)
    // NOTE: We're using AddressLayout::High as a test here. It should just shadow memory such as the heap or any buffers.
    mapShadowMemory(VirtualAddress(AddressLayout::High), kasanHighShadowSize);
    unpoison(VirtualAddress(AddressLayout::High), kasanHighShadowSize);
    logLine << "KASAN: Initialized\n"
            << fmt::endl;
    kasan_initialized = true;
#else
    logLine << "KASAN is disabled\n"
            << fmt::endl;
#endif
}

[[gnu::no_sanitize_address]] void poison(VirtualAddress addr, size_t size, ShadowRegionStatus status) {
#if defined(FIREFLY_KASAN)
    assert_truth(!kasanUserAddress(addr) && "Kasan cannot poison user pages yet!");
    memset(static_cast<void *>(toShadow(addr)), static_cast<uint8_t>(status), size >> kasanShift);
#else
    (void)addr;
    (void)size;
    (void)status;
#endif
}

[[gnu::no_sanitize_address]] void unpoison(VirtualAddress addr, size_t size) {
#if defined(FIREFLY_KASAN)
    poison(addr, size, ShadowRegionStatus::Unpoisoned);
#else
    (void)addr;
    (void)size;
#endif
}

[[gnu::no_sanitize_address]] inline bool isPoisoned(KasanAddress addr, size_t size) {
    auto shadow = toShadow(VirtualAddress(addr));
    for (size_t i = 0; i < (size >> kasanShift); i++) {
        if (shadow[i] == static_cast<uint8_t>(ShadowRegionStatus::Poisoned))
            return true;
    }

    return false;
}

[[gnu::no_sanitize_address]] void verifyAccess(KasanAddress addr, size_t size, bool write) {
    if (!kasan_initialized || !withinKasanMemory(addr))
        return;

    // TODO: This should be offered by console.hpp or logger.hpp
    constexpr auto color_red = "\033[31m";
    constexpr auto color_reset = "\033[39m";

    // Dump 'len' bytes of shadow memory and highlight all bytes in a range from shadow_bytes_{start,end}.
    const auto dumpShadowMemory = [=](KasanAddress base, size_t len, auto shadow_bytes_start, auto shadow_bytes_end) {
        logLine << fmt::hex << reinterpret_cast<uintptr_t>(fromShadow(base)) << ": ";
        for (size_t i = 0; i < len; i++) {
            logLine << ((base + i) >= shadow_bytes_start && (base + i) <= shadow_bytes_end ? color_red : color_reset)
                    << fmt::hex << (*reinterpret_cast<int8_t *>(base + i) & 0xff) << ' ';
        }

        logLine << color_reset << '\n'
                << fmt::endl;
    };

    static int recursion;
    if (++recursion == 1) {
        if (isPoisoned(addr, size)) {
            logLine << "\n==== " << color_red << "Kasan detected a memory access violation" << color_reset << " ====\n"
                    << (write ? "Write to " : "Read from ") << "poisoned address " << fmt::hex << addr << " of size " << size << '\n'
                    << fmt::endl;

            logLine << "Memory state around the buggy address:\n"
                    << fmt::endl;

            // Shadow memory at the address `start => end` is the poisoned memory.
            auto shadow = uint64_t(toShadow(VirtualAddress(addr)));
            const KasanAddress shadow_bytes_start = shadow;
            const KasanAddress shadow_bytes_end = shadow + (size >> kasanShift);
            if constexpr (debugKasan)
                logLine << "KASAN-DBG: shadow_start=" << fmt::hex << shadow_bytes_start << ", shadow_end=" << fmt::hex << shadow_bytes_end << '\n'
                        << fmt::endl;

            logLine << '\n'
                    << fmt::endl;
            bool had_to_break = false;
            for (int i = 0; i < 3; i++) {
                if ((AddressLayout::High + (kasanDumpSize * i)) >= addr) {
                    had_to_break = true;
                    break;
                }

                dumpShadowMemory(KasanAddress(toShadow(VirtualAddress(AddressLayout::High + (kasanDumpSize * i)))), kasanDumpSize, shadow_bytes_start, shadow_bytes_end);
            }
            if (!had_to_break)
                logLine << "<Omitting " << (addr - AddressLayout::High) / kasanDumpSize << " shadow frame(s)>\n"
                        << fmt::endl;

            dumpShadowMemory(shadow, kasanDumpSize, shadow_bytes_start, shadow_bytes_end);
            panic("kasan access violation");
        }
    }
    recursion--;
}
}  // namespace firefly::kernel::kasan
