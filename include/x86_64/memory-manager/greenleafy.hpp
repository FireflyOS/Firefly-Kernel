#pragma once

#include <stl/cstdlib/cstdint.h>

namespace firefly::mm::greenleafy {
    #define BLOCK_LIMIT 1024
    #define BLOCK_SIZE_LIMIT 8192

    void *alloc_block(uint32_t access);
    void *get_block(uint64_t block_number, uint32_t access);
    
    uint32_t get_block_limit();
    uint32_t get_block_size_limit();
}