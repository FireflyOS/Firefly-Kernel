#pragma once

#include <stl/cstdlib/cstdint.h>

namespace firefly::kernel::virtual_memory {
    uint8_t *create_virtual_memory();
    uint8_t *get_memory_location(uint8_t *virtual_memory_instance, int element_number);
}