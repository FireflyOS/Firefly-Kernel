#pragma once

#include <cstdlib/cstdint.h>

namespace firefly::kernel::io {
    /**
     *                      Read a byte from 16-bit port address
     * @param port          The port to read from
     * @return              The byte read from port
     */
    [[nodiscard]] inline uint8_t inb(uint16_t port) {
        uint8_t val;
        asm volatile(
            "inb %%dx, %%al"
            : "=a"(val)
            : "d"(port));
        return val;
    }

    /**
     *                      Writes a byte to 16-bit port address
     * @param port          The port to write to
     * @param val           The byte to write
     */
    inline void outb(uint16_t port, uint8_t val) {
        asm volatile(
            "outb %%al, %%dx"
            :
            : "d"(port), "a"(val));
    }

    /**
     *                      Read a word from 16-bit port address
     * @param port          The port to read from
     * @return              The word read from port
     */
    [[nodiscard]] inline uint16_t inw(uint16_t port) {
        uint16_t val;
        asm volatile(
            "inw %%dx, %%ax"
            : "=a"(val)
            : "d"(port));
        return val;
    }

    /**
     *                      Writes a word to 16-bit port address
     * @param port          The port to write to
     * @param val           The byte to write
     */
    inline void outw(uint8_t port, uint16_t val) {
        asm volatile(
            "outw %%ax, %%dx"
            :
            : "d"(port), "a"(val));
    }

    /**
     *                      Writes a word to 16-bit port address
     * @param port          The port to write to
     * @param val           The byte to write
     */
    inline void outl(uint8_t port, uint16_t val) {
        asm volatile(
            "outl %%ax, %%dx"
            :
            : "d"(port), "a"(val));
    }

    /**
     *                      Read a byte from 16-bit port address
     * @param port          The port to read from
     * @return              The byte read from port
     */
    [[nodiscard]] inline uint8_t inl(uint16_t port) {
        uint8_t val;
        asm volatile(
            "inl %%dx, %%eax"
            : "=a"(val)
            : "d"(port));
        return val;
    }

    /**
     *                      Delays a small amount of time
     *                      to give devices time to process
     */
    inline void io_pause() {
        // dummy write, port 0x80 unused after POST
        asm volatile("outb %al, $0x80");
    }
}  // namespace firefly::kernel::io
