#include <stl/cstdlib/cstdint.h>
#include <x86_64/memory-manager/greenleafy.hpp>
#include <x86_64/drivers/serial_legacy.hpp>

namespace firefly::mm::greenleafy {
    uint8_t blocks[1024][8192];
    uint64_t current_block = 0;
    uint64_t block_access[1024];


    void *alloc_block(uint64_t access) {
        block_access[current_block] = access;
        current_block++;
        kernel::io::legacy::writeTextSerial("New block was allocated! Address: 0x%X\n\n", blocks[current_block]);
        return blocks[current_block];
    }

    
    void *get_block(uint64_t block_number, uint64_t access){
        if(block_access[block_number] < access || block_access[block_number] == access){
            return blocks[block_number];
        } else {
            return NULL;
        }
    }


    //TODO: Почини этот CPU Exception caugth
          //CPU Exception caught
          // CS: 0xffffffff80203fc6
          //EIP: 2
          //ESP: 86
}