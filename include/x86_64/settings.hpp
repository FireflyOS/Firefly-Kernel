#pragma once

#include <stl/cstdlib/cstdint.h>

namespace firefly::kernel::settings {
    extern uint8_t kernel_settings[8];

    /*
    Here you can change settings of Firefly Kernel! => <dynnfy  >
    Kernel Settings:
    - 1. Kernel Mode ('d')debug, ('p')production
    - 2. Enable Serial Port ('y')yes, ('n')no
    - 3. Disable Application Access Rights ('y')yes, ('n')no
    - 4. Disable Memory Block Access Rights ('y')yes, ('n')no
    - 5. Limit Block Count to ('h')512 or ('l')256 or ('f')1024
    - 6. Print interrupts and exceptions on serial port too. ('y')yes, ('n')no
    - 7-8. Reserved
    */
    extern char *instruction;

    /*
        Loads settings
    */
    void init_settings();
}