#pragma once
#include "cstdlib/cstdint.h"
#include "cstdlib/stdio.h"

namespace firefly::libkern {
inline static char buff[20];
inline char* hex(int n) {
    return itoa(n, buff, 16);
}

inline char* dec(int n) {
    return itoa(n, buff, 10);
}
}  // namespace firefly::libkern