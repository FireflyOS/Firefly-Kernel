#include "x86_64/memory-manager/virtual/virtual.hpp"
#include "x86_64/memory-manager/primary/primary_phys.hpp"
#include "x86_64/trace/strace.hpp"
#include "x86_64/memory-manager/relocation.hpp"
#include "x86_64/fb/stivale2-term.hpp"
#include <stl/cstdlib/stdio.h>

namespace firefly::kernel::mm
{
    using namespace firefly::mm::relocation::conversion;
    using namespace mm;

    constexpr size_t GB = 0x40000000UL;

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

        auto pml4 = pmm::allocate();
        if (pml4 == nullptr) trace::panic("Failed to allocate memory for the kernel pml4");
        this->kernel_pml4 = static_cast<pte_t*>(pml4);

        for (size_t n = 0; n < GB * 4; n += PAGE_SIZE)
        {
            this->map(n, n, 0x3);
        }

        // for (size_t i = 0; i < mmap->entries; i++)
        // {
        //     if (mmap->memmap[i].type == STIVALE2_MMAP_BAD_MEMORY) {continue;}

        //     // Note: These addresses are guaranteed to page aligned
        //     auto addr_base = mmap->memmap[i].base;
        //     auto addr_len = mmap->memmap[i].length;

        //     // Check if we need to map these entries in order to use the stivale2 terminal.
        //     // We identity mapped 1 GiB of lower memory, if that doesn't cover the two entries below we map them.
        //     if (addr_base + addr_len < GB)
        //         continue;

        //     if (mmap->memmap[i].type == STIVALE2_MMAP_FRAMEBUFFER ||
        //         mmap->memmap[i].type == STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE)
        //     {
        //         for (auto i = addr_base; i < addr_base + addr_len; i += PAGE_SIZE)
        //         {
        //             this->map(i, i, 0x3); //Read write
        //         }
        //     }
        // }

        for (size_t n = 0; n < 2* GB; n += PAGE_SIZE)
        {
            this->map(n, to_higher_half(n, DATA), 0x3);
        }

        for (size_t base = 0; base < 2 * GB; base += PAGE_SIZE)
        {
            this->map(base, to_higher_half(base, CODE), 0x3);
        }

        asm volatile("mov %0, %%cr3\n" :: "r"(this->kernel_pml4));
        printf("vmm: Initialized\n");
    }

    walk_t VirtualMemoryManager::walk(virt_t virtual_addr, pte_t *pml_ptr, uint64_t access_flags)
    {
        auto idx4 = this->get_index(virtual_addr, 4);
        auto idx3 = this->get_index(virtual_addr, 3);
        auto idx2 = this->get_index(virtual_addr, 2);
        auto idx1 = this->get_index(virtual_addr, 1);

        if (!(pml_ptr[idx4] & 1)) {
            auto ptr = pmm::allocate();
            if (ptr == nullptr) trace::panic("OOM");
            this->kernel_pml4[idx4] = reinterpret_cast<pte_t>(ptr);
            this->kernel_pml4[idx4] |= access_flags;
        }
        auto pml3 = (pte_t*)(this->kernel_pml4[idx4] & ~(511));
        
        if (!(pml3[idx3] & 1)) {
            auto ptr = pmm::allocate();
            if (ptr == nullptr) trace::panic("OOM");
            pml3[idx3] = reinterpret_cast<pte_t>(ptr);
            pml3[idx3] |= access_flags;
        }
        auto pml2 = (pte_t*)(pml3[idx3] & ~(511));

        if ((pml2[idx2] & 1) == 0) {
            auto ptr = pmm::allocate();
            if (ptr == nullptr) trace::panic("OOM");
            pml2[idx2] = reinterpret_cast<pte_t>(ptr);
            pml2[idx2] |= access_flags;
        }
        return { 
            .idx = idx1,
            .pml1 = (pte_t*)(pml2[idx2] & ~(511))
        };
    }

    void VirtualMemoryManager::map(phys_t physical_addr, virt_t virtual_addr, uint64_t access_flags, pte_t *pml_ptr)
    {
        if (pml_ptr == nullptr) { pml_ptr = this->kernel_pml4; }

        auto table_walk_result = walk(virtual_addr, pml_ptr, access_flags);

        // Note: This only works because allocate() memsets the addresses data to 0
        // if (table_walk_result.pml1[table_walk_result.idx] != 0x0) trace::panic("Attempted to map mapped page");
        table_walk_result.pml1[table_walk_result.idx] = (physical_addr | access_flags);
    }

    void VirtualMemoryManager::remap(virt_t virtual_addr_old, virt_t virtual_addr_new, uint64_t access_flags, pte_t *pml_ptr)
    {
        /* Unmap page */
        auto table_walk_result = walk(virtual_addr_old, pml_ptr, 0);
        auto phys_addr = table_walk_result.pml1[table_walk_result.idx];
        table_walk_result.pml1[table_walk_result.idx] = 0;
        asm volatile("invlpg %0" :: "m"(virtual_addr_old));

        this->map(phys_addr, virtual_addr_new, access_flags, pml_ptr);
    }

} // namespace firefly::kernel::mm