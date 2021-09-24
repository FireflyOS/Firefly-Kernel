#pragma once

#include <stl/cstdlib/cstdint.h>

namespace firefly::mm::greenleafy {
    void *alloc_block(uint64_t access);
    void *get_block(uint64_t block_number, uint64_t access);
}