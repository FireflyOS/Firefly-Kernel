#pragma once

#include <stl/cstdlib/cstdint.h>

namespace firefly::kernel::settings {
    /*
        Loads settings
    */
    void init_settings();

    namespace get {
        uint8_t kernel_mode(void);
        uint8_t enable_serial_port(void);
        uint8_t disable_app_access_rights(void);
        uint8_t disable_memory_block_access_rights(void);
        uint8_t block_count(void);
    }
}