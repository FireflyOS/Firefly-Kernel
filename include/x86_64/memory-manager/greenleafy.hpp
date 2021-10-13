#pragma once

#include <stl/cstdlib/cstdint.h>

namespace firefly::mm::greenleafy {
    #define BLOCK_LIMIT 1024
    #define BLOCK_SIZE_LIMIT 8192

    struct memory_block {
        unsigned char block[BLOCK_SIZE_LIMIT];
        uint32_t      block_access;
        uint64_t      block_number;
        unsigned char is_used;
    }; //(BLOCK_SIZE_LIMIT + 14) bytes per block 

    void *use_block(uint32_t access);
    void *get_block(uint64_t block_number, uint32_t access);
    
    uint32_t get_block_limit();
    uint32_t get_block_size_limit();
}