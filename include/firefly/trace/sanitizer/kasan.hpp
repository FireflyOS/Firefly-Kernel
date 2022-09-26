#pragma once

#include <cstddef>
#include <cstdint>

#include "firefly/memory-manager/mm.hpp"

namespace firefly::kernel::kasan {
using KasanAddress = uintptr_t;

/* Each byte of the shadow region encodes the status of the address it shadows */
enum ShadowRegionStatus : int8_t {
    // TODO: Add more region states such as a redzone for buffer overruns
    Unpoisoned = 0,
    Poisoned = ~0
};

[[gnu::no_sanitize_address]] void init();
[[gnu::no_sanitize_address]] void poison(VirtualAddress addr, size_t size, ShadowRegionStatus status = ShadowRegionStatus::Poisoned);
[[gnu::no_sanitize_address]] void unpoison(VirtualAddress addr, size_t size);

#if defined(FIREFLY_KASAN)
[[gnu::no_sanitize_address]] void verifyAccess(KasanAddress addr, size_t size, bool write);
#endif
}  // namespace firefly::kernel::kasan