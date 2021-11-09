#pragma once

#include <stdint.h>
#include <stddef.h>

namespace firefly::kernel::mm
{
    typedef uint64_t pte_t;
    typedef uint64_t virt_t;
    typedef uint64_t phys_t;

    constexpr size_t page_offset = 12; //Lower 12 bits of a virtual address denote the offset in the page frame - We don't need that
    
    class VirtualMemoryManager
    {
        public:
            VirtualMemoryManager();

        public:
            void map(phys_t physical_addr, virt_t virtual_addr, int access_flags);
            inline pte_t* get_kernel_pml4() { return this->kernel_pml4; }

        private:
            pte_t *kernel_pml4;
        
        private:
            int64_t get_index(virt_t virtual_addr, const int idx) { 
                // 9 bits per index - 9 * idx = index 'idx' in virtual_address, skip the first 12 bits
                return (virtual_addr >> (page_offset + (9 * (idx-1)))) & 0x1FF; // We subtract 1 from idx so that we don't have to input idx 0-3, but rather 1-4
            }

            pte_t* next_table(pte_t* (*fn)()) {
                return fn();
            }
    };
} // namespace firefly::kernel::mm
