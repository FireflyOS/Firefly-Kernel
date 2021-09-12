#include <stl/utility.h>
#include <x86_64/libk++/iostream.h>

#include <x86_64/utils.hpp>

namespace firefly::kernel::util {
uint32_t rev32(uint32_t bytes) {
    return ((bytes & 0xFF) << 16) | (((bytes >> 8) & 0xFF) << 8) | (((bytes >> 16) & 0xFF));
}

//Converts a single (B,G,R)(array) color pair into an RGB value (uint32_t)
uint32_t bgr2rgb(uint32_t bytes[], int offset) {
    uint32_t concatenated = (bytes[offset + 0] << 16) | (bytes[offset + 1] << 8) | (bytes[offset + 2]);
    return rev32(concatenated);
}
}  // namespace firefly::kernel::util