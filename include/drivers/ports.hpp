#include <cstdlib/cstdint.h>

namespace firefly {
    inline uint8_t read_port(uint16_t port) {
        uint8_t val;
        asm volatile("inb %1, %0"
                     : "=a"(val)
                     : "Nd"(port));
        return val;
    }
}  // namespace firefly