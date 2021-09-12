#pragma once
#include <stl/cstdlib/cstdint.h>

#define VIRT_BASE 0xFFFFFFFF80000000UL

namespace firefly::mm::relocation {
namespace conversion {
inline size_t to_higher_half(size_t address) {
    return (address + VIRT_BASE);
}
} // namespace conversion
}  // namespace conversion
