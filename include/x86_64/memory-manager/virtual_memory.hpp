#pragma once

namespace firefly::kernel::virtual_memory {
    uint8_t *create_virtual_memory();
    uint8_t *get_memory_location(uint8_t *virtual_memory_instance, unsigned long element_number);
    void multiple_set(uint8_t *virtual_memory_instance, uint8_t *bytes, unsigned long start_point);
    uint16_t get_16bit_number(uint8_t *virtual_memory_instance, unsigned long start_point);
    uint32_t get_32bit_number(uint8_t *virtual_memory_instance, unsigned long start_point);
}