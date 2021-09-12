#pragma once
#include <stl/cstdlib/cstdint.h>

#include "x86_64/memory-manager/buddy/buddy.hpp"
#include "x86_64/stivale2.hpp"

namespace firefly::kernel::mm::buddy {
// static BuddyAllocator Buddy{ nullptr };
void bootstrap_buddy(struct stivale2_struct_tag_memmap *phys_mmap);
static inline uint64_t ram_diff(struct stivale2_mmap_entry *mmap_entries, int total_entries);
}  // namespace firefly::kernel::mm::buddy
