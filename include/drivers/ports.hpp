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

    [[nodiscard]] inline uint8_t inb(uint8_t port) {
        return read_port(port);
    }

    inline void outb(uint8_t port, uint8_t val) {
        write_port(port, val);
    }


    inline void wait_for_io() {
        /* Port 0x80 is used for 'checkpoints' during POST. */
        /* The Linux kernel seems to think it is free for use :-/ */
        asm volatile("outb %%al, $0x80"
                     :
                     : "a"(0));
        /* %%al instead of %0 makes no difference.  TODO: does the register need to be zeroed? */
    }
}  // namespace firefly