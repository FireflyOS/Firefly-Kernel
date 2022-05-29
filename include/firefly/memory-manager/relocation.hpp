#pragma once
#include <cstddef>

#define KERNEL_CODE_OFFSET 0xFFFFFFFF80000000UL
#define KERNEL_DATA_OFFSET 0xFFFF800000000000UL

namespace firefly::mm::relocation {
namespace conversion {
enum ADDRESS_TYPE {
    CODE = 1,  // Use 'KERNEL_CODE_OFFSET' in conversions to/from higher half addresses
    DATA = 2   // Use 'KERNEL_DATA_OFFSET' in conversions to/from higher half addresses
};
inline size_t to_higher_half(size_t address, enum ADDRESS_TYPE addr_type) {
    if (addr_type == CODE)
        return (address + KERNEL_CODE_OFFSET);

    else if (addr_type == DATA)
        return (address + KERNEL_DATA_OFFSET);

    return 0;
}

inline size_t from_higher_half(size_t address, enum ADDRESS_TYPE addr_type) {
    if (addr_type == CODE)
        return (address - KERNEL_CODE_OFFSET);

    else if (addr_type == DATA)
        return (address - KERNEL_DATA_OFFSET);

    return 0;
}
}  // namespace conversion
}  // namespace firefly::mm::relocation
