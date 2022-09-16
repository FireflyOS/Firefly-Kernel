#pragma once

#include <cstddef>
#include <cstdint>

namespace firefly::kernel::kasan {
void init();
[[gnu::no_sanitize_address]] void reportFailure(uintptr_t addr, size_t size, bool write);
}  // namespace firefly::kernel::kasan