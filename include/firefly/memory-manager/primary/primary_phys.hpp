#pragma once

#include <cstdlib/cstring.h>
#include <stdint.h>

#include <frg/list.hpp>

#include "firefly/logger.hpp"
#include "firefly/memory-manager/mm.hpp"
#include "firefly/memory-manager/zone-specifier.hpp"
#include "firefly/stivale2.hpp"
#include "firefly/trace/strace.hpp"

namespace firefly::kernel::mm::pmm {
void init(stivale2_struct_tag_memmap *mmap);
PhysicalAddress allocate(uint64_t size = 4096, FillMode fill = FillMode::ZERO);
void deallocate(PhysicalAddress ptr);
};  // namespace firefly::kernel::mm::pmm