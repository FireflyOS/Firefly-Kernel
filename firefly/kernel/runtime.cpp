#include <cstddef>

#include "firefly/memory-manager/primary/primary_phys.hpp"

void* operator new(size_t size) {
    return firefly::kernel::mm::Physical::allocate(size);
}
