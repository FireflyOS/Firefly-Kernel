#include <stl/cstdlib/cstdint.h>
#include <x86_64/memory-manager/greenleafy.hpp>
#include <x86_64/drivers/serial_legacy.hpp>
#include <x86_64/settings.hpp>
#include <x86_64/trace/strace.hpp>

namespace firefly::mm::greenleafy {
    uint8_t blocks[BLOCK_LIMIT][BLOCK_SIZE_LIMIT];
    uint64_t current_block = 0;
    uint32_t block_access[BLOCK_LIMIT];

    void *alloc_block(uint32_t access) {
        if(current_block == 512 && firefly::kernel::settings::get::block_count() == 0x51) return NULL;
        if(current_block == 256 && firefly::kernel::settings::get::block_count() == 0x25) return NULL;
        if(current_block == BLOCK_LIMIT && firefly::kernel::settings::get::block_count() == 0x12) {
            firefly::trace::panic(firefly::trace::PM_NO_BLOCKS_AVALIABLE, firefly::trace::PC_NO_BLOCKS_AVALIABLE);
        }

        block_access[current_block] = (firefly::kernel::settings::get::disable_memory_block_access_rights() == 0x00) ? access : 0;
        kernel::io::legacy::writeTextSerial("New block was allocated! Address: 0x%X\n\n", blocks[current_block]);
        current_block++;
        return blocks[current_block];
    }
    
    void *get_block(uint64_t block_number, uint32_t access){
        if((block_access[block_number] < access || block_access[block_number] == access) && block_number < 1024){
            return blocks[block_number];
        } else {
            return NULL;
        }
    }

    uint32_t get_block_limit() { return BLOCK_LIMIT; };
    uint32_t get_block_size_limit() { return BLOCK_SIZE_LIMIT; };
    
}