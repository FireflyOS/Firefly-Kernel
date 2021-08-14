#pragma once
#include <stl/cstdlib/cstdint.h>

#include <i386/multiboot2.hpp>

namespace firefly::mm::buddy {
void bootstrap_buddy(multiboot_uint64_t &mmap_base_entry, multiboot_uint64_t &length);
}  // namespace firefly::mm::buddy
