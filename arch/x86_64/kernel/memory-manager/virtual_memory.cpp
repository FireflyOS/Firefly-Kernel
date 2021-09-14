#include <stl/cstdlib/cstdint.h>
#include <x86_64/memory-manager/virtual_memory.hpp>
#include <x86_64/drivers/serial_legacy.hpp>

namespace firefly::kernel::virtual_memory {
    uint8_t *create_virtual_memory(){
        static uint8_t virtual_mem[8192];
        io::legacy::writeTextSerial("A new Virtual Memory Instance was made.\n");
        return virtual_mem;
    }
    uint8_t *get_memory_location(uint8_t *virtual_memory_instance, unsigned long element_number){
        return virtual_memory_instance + element_number;
    }
    void multiple_set(uint8_t *virtual_memory_instance, uint8_t *bytes, unsigned long start_point){
        unsigned long i = 0;
        while(i < sizeof(bytes)){
            virtual_memory_instance[start_point + i] = bytes[i];
            i++;
        }
    }
    uint16_t get_16bit_number(uint8_t *virtual_memory_instance, unsigned long start_point) {
        return virtual_memory_instance[start_point + 0] | (virtual_memory_instance[start_point + 1] << 8);
    }
    uint32_t get_32bit_number(uint8_t *virtual_memory_instance, unsigned long start_point){
        return virtual_memory_instance[start_point + 0] | (virtual_memory_instance[start_point + 1] << 8) | (virtual_memory_instance[start_point + 2] << 16) | (virtual_memory_instance[start_point + 3] < 24);
    }
}