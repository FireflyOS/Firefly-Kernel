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
enum FillMode : char {
    ZERO = 0,
    NONE = 1  // Don't fill
    // Todo: Asan fill mode?
};

void init(stivale2_struct_tag_memmap *mmap);
PhysicalAddress allocate(FillMode fill = FillMode::ZERO);
void deallocate(PhysicalAddress ptr);
};  // namespace firefly::kernel::mm::pmm