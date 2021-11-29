#include "x86_64/memory-manager/virtual/virtual.hpp"
#include "x86_64/memory-manager/primary/primary_phys.hpp"
#include "x86_64/trace/strace.hpp"
#include "x86_64/memory-manager/relocation.hpp"
#include "x86_64/fb/stivale2-term.hpp"
#include <stl/cstdlib/stdio.h>

// Kernel mapping is based on these values (Defined in the linker script)
extern size_t KERNEL_IMAGE_BASE[];
extern size_t KERNEL_IMAGE_TOP [];

namespace firefly::kernel::mm
{
    using namespace firefly::mm::relocation::conversion;
    using namespace mm::primary;

    VirtualMemoryManager::VirtualMemoryManager(bool initial_mapping, stivale2_struct_tag_memmap *memory_map)
    {
        if (initial_mapping)
        {
            this->configure_initial_kernel_mapping(memory_map);
        }
    }

    void VirtualMemoryManager::configure_initial_kernel_mapping(stivale2_struct_tag_memmap *mmap)
    {
        if (mmap == nullptr) trace::panic("The mmap argument passed was a nullptr!");

        auto pml4 = allocate(1);
        if (pml4 == nullptr) trace::panic("Failed to allocate memory for the kernel pml4");
        this->kernel_pml4 = static_cast<pte_t*>(pml4->data[0]);

        for (size_t i = 0; i < mmap->entries; i++)
        {
            if (mmap->memmap[i].type == STIVALE2_MMAP_BAD_MEMORY) {continue;}

            // Note: These addresses are guaranteed to page aligned
            auto addr_base = mmap->memmap[i].base;
            auto addr_len = mmap->memmap[i].length;

            // Generic memory entries
            if (mmap->memmap[i].type == STIVALE2_MMAP_USABLE || 
                mmap->memmap[i].type == STIVALE2_MMAP_ACPI_RECLAIMABLE ||
                mmap->memmap[i].type == STIVALE2_MMAP_ACPI_NVS
            )
            {
                for (auto i = addr_base; i < addr_base + addr_len; i += PAGE_SIZE)
                {
                    this->map(i, to_higher_half(i, DATA), 0x3); //Read write
                }
            }
            // Unusable entries
            else if (mmap->memmap[i].type == STIVALE2_MMAP_RESERVED)
            {
                for (auto i = addr_base; i < addr_base + addr_len; i += PAGE_SIZE)
                {
                    this->map(i, to_higher_half(i, DATA), 0x1); //Read only
                }
            }
            // Framebuffer and bootloader reclaimable memory
            else
            {
                for (auto i = addr_base; i < addr_base + addr_len; i += PAGE_SIZE)
                {
                    this->map(i, i, 0x3); //Read write
                }
            }
        }

        auto base = reinterpret_cast<size_t>(KERNEL_IMAGE_BASE);
        auto top  = reinterpret_cast<size_t>(KERNEL_IMAGE_TOP);
        for (; base < top; base += PAGE_SIZE)
        {
            this->map(base, to_higher_half(base, CODE), 0x3);
        }

        asm volatile("mov %0, %%cr3\n" :: "r"(this->kernel_pml4));
    }

    void VirtualMemoryManager::map(phys_t physical_addr, virt_t virtual_addr, uint64_t access_flags, pte_t *pml_ptr)
    {
        if (pml_ptr == nullptr) { pml_ptr = this->kernel_pml4; }

        auto idx4 = this->get_index(virtual_addr, 4);
        auto idx3 = this->get_index(virtual_addr, 3);
        auto idx2 = this->get_index(virtual_addr, 2);
        auto idx1 = this->get_index(virtual_addr, 1);

        if (!(pml_ptr[idx4] & 1)) {
            auto ptr = allocate(1);
            if (ptr == nullptr) trace::panic("OOM");
            this->kernel_pml4[idx4] = reinterpret_cast<pte_t>(ptr->data[0]);
            this->kernel_pml4[idx4] |= access_flags;
        }
        auto pml3 = (pte_t*)(this->kernel_pml4[idx4] & ~(511));
        
        if (!(pml3[idx3] & 1)) {
            auto ptr = allocate(1);
            if (ptr == nullptr) trace::panic("OOM");
            pml3[idx3] = reinterpret_cast<pte_t>(ptr->data[0]);
            pml3[idx3] |= access_flags;
        }
        auto pml2 = (pte_t*)(pml3[idx3] & ~(511));

        if ((pml2[idx2] & 1) == 0) {
            auto ptr = allocate(1);
            if (ptr == nullptr) trace::panic("OOM");
            pml2[idx2] = reinterpret_cast<pte_t>(ptr->data[0]);
            pml2[idx2] |= access_flags;
        }
        auto pml1 = (pte_t*)(pml2[idx2] & ~(511));    

        // Note: This only works because allocate() memsets the addresses data to 0
        if (pml1[idx1] != 0x0) trace::panic("Attempted to map mapped page");
        pml1[idx1] = (physical_addr | access_flags);
    }

} // namespace firefly::kernel::mm