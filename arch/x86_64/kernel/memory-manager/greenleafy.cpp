#include <stl/cstdlib/cstdint.h>
#include <x86_64/memory-manager/greenleafy.hpp>
#include <x86_64/drivers/serial_legacy.hpp>
#include <x86_64/settings.hpp>
#include <x86_64/trace/strace.hpp>

namespace firefly::mm::greenleafy {
    memory_block memory_blocks[BLOCK_LIMIT];

    uint64_t current_block = 0;

    void *use_block(uint32_t access) {
        if(current_block == 512 && firefly::kernel::settings::get::block_count() == 0x51) return NULL;
        if(current_block == 256 && firefly::kernel::settings::get::block_count() == 0x25) return NULL;
        if(current_block == BLOCK_LIMIT && firefly::kernel::settings::get::block_count() == 0x12) {
            firefly::trace::panic(firefly::trace::PM_NO_BLOCKS_AVALIABLE, firefly::trace::PC_NO_BLOCKS_AVALIABLE);
        }

        memory_blocks[current_block].block_access = access;
        memory_blocks[current_block].is_used = 1;
        memory_blocks[current_block].block_number = current_block;
        
        kernel::io::legacy::writeTextSerial("New block was used! Address: 0x%X\n\n", &memory_blocks[current_block]);

        return &memory_blocks[current_block++];

    }
    
    void *get_block(uint64_t block_number, uint32_t access){
        if((memory_blocks[block_number].block_access < access || memory_blocks[block_number].block_access == access) && block_number < 1024){
            return &memory_blocks[block_number];
        } else {
            return NULL;
        }
    }

    uint32_t get_block_limit() { return BLOCK_LIMIT; };
    uint32_t get_block_size_limit() { return BLOCK_SIZE_LIMIT; };
    
}