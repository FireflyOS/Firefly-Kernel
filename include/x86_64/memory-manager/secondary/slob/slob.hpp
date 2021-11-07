#pragma once

#include <stddef.h>
#include "x86_64/memory-manager/primary/primary_phys.hpp"

namespace firefly::kernel::mm::slob {
constexpr size_t SLOB_MAX_ALLOC = mm::primary::PAGE_SIZE; /* Don't support < 4KiB for now */

class Slob {
    void initialize();
    void allocate(size_t size /* Todo: Access flags? */);
};
}  // namespace firefly::kernel::mm::slob