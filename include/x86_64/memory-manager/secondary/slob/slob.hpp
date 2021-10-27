#pragma once

#include <stddef.h>

namespace firefly::kernel::mm::slob {
constexpr const int SLOB_MAX_ALLOC = 0x1000; /* Don't support < 4KiB for now */

class Slob {
    void initialize();
    void allocate(size_t size /* Todo: Acces flags? */);
};
}  // namespace firefly::kernel::mm::slob