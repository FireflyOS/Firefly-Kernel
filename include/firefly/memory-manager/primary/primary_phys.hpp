#pragma once

#include <stdint.h>

#include "firefly/memory-manager/mm.hpp"
#include "firefly/stivale2.hpp"

namespace firefly::kernel::mm::Physical {
void init(stivale2_struct_tag_memmap *mmap);
PhysicalAddress allocate(uint64_t size = 4096, FillMode fill = FillMode::ZERO);
PhysicalAddress must_allocate(uint64_t size = 4096, FillMode fill = FillMode::ZERO);
void deallocate(PhysicalAddress ptr);
}  // namespace firefly::kernel::mm::Physical