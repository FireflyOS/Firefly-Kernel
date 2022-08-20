#pragma once

#include <stdint.h>

namespace firefly::libkern {
template <typename T = uint64_t>
inline T align_up4k(T in) {
    return (in + 4096 - 1) & ~(4096 - 1);
}

template <typename T = uint64_t>
inline T align_down4k(T in) {
    return (in) & ~(4096 - 1);
}
}  // namespace firefly::libkern
