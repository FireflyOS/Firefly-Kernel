#pragma once

#include <cstddef>
#include <cstdint>

namespace firefly::kernel::kasan {
using KasanAddress = uintptr_t;

void init();
[[gnu::no_sanitize_address]] void poison(KasanAddress addr);
[[gnu::no_sanitize_address]] void unpoison(KasanAddress addr);
[[gnu::no_sanitize_address]] inline bool isPoisoned(KasanAddress addr);
[[gnu::no_sanitize_address]] void verifyAccess(KasanAddress addr, size_t size, bool write);
}  // namespace firefly::kernel::kasan