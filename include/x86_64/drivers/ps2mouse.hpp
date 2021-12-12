#pragma once

#include <stl/cstdlib/cstdint.h>
#include <x86_64/libk++/iostream.h>

#include <x86_64/drivers/ports.hpp>

#include <x86_64/int/interrupt.hpp>

#define PS2MOUSE_PORT 0x60

namespace firefly::kernel::io::mouse {
    namespace variables {
        extern uint8_t cycle;
        extern int8_t buffer[3];
        extern int8_t x;
        extern int8_t y;
    }
    void handle(iframe *intframe);
    void wait(uint8_t is_write);
    void write(uint8_t byte);
    uint8_t read();
    void init();
}