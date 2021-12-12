#include <stl/cstdlib/cstdint.h>
#include <x86_64/memory-manager/greenleafy.hpp>
#include <x86_64/drivers/serial_legacy.hpp>
#include <x86_64/settings.hpp>
#include <x86_64/trace/strace.hpp>

namespace firefly::mm::greenleafy {
    memory_block memory_blocks[BLOCK_LIMIT];

    memory_block *use_block(uint32_t access) {
        //search free blocks

        uint64_t block_num = 0;
        while(memory_blocks[block_num].is_used == 1){
            block_num++;
        }

        memory_blocks[block_num].block_access = access;
        memory_blocks[block_num].is_used = 1;
        memory_blocks[block_num].block_number = block_num;

        int i = 0;
        while(i < BLOCK_SIZE_LIMIT) memory_blocks[block_num].block[i++] = 0;
        
        kernel::io::legacy::writeTextSerial("New block was used (%d)! Address: 0x%X\n\n", block_num, &memory_blocks[block_num]);

        return &memory_blocks[block_num];

    }

    void delete_block(uint64_t block_number){
        int i = 0;
        while(i < BLOCK_SIZE_LIMIT) memory_blocks[block_number].block[i++] = 0;

        memory_blocks[block_number].is_used = 0;
    }
    
    memory_block *get_block(uint64_t block_number, uint32_t access){
        if((memory_blocks[block_number].block_access < access || memory_blocks[block_number].block_access == access) && block_number < 1024){
            return &memory_blocks[block_number];
        } else {
            return NULL;
        }
    }

    uint32_t get_block_limit(void) { return BLOCK_LIMIT; };
    uint32_t get_block_size_limit(void) { return BLOCK_SIZE_LIMIT; };
    
}