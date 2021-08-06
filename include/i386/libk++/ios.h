#pragma once
#include "cstdlib/cstdint.h"
#include "cstdlib/stdio.h"

namespace firefly::libkern {
char buff[20];
char* hex(int n) {
    return itoa(n, buff, 16);
}

char* dec(int n) {
    return itoa(n, buff, 10);
}
}  // namespace firefly::libkern