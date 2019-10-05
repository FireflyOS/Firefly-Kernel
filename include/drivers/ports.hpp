#include <cstdlib/cstdint.h>

namespace firefly {
    [[nodiscard]] inline uint8_t read_port(uint8_t port) {
        asm volatile("inb %1, %0"
                     : "=a"(port)
                     : "Nd"(port));
        return port;
    }

    inline void write_port(uint8_t port, uint8_t val) {
        asm volatile(
            "outb %%al, %0"
            :
            : "i"(port), "a"(val));
    }
}  // namespace firefly