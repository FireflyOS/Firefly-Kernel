#include "x86_64/memory-manager/virtual/virtual.hpp"
#include "x86_64/memory-manager/primary/primary_phys.hpp"
#include "x86_64/trace/strace.hpp"
#include "x86_64/memory-manager/relocation.hpp"

#include <stl/cstdlib/stdio.h>

namespace firefly::kernel::mm
{
    using namespace firefly::mm::relocation::conversion;
    using namespace mm::primary;

    constexpr size_t GB = 0x40000000;

    VirtualMemoryManager::VirtualMemoryManager()
    {
        // Setup PML4
        auto pml4 = allocate(1);
        if (pml4 == nullptr) trace::panic("Failed to allocate memory!");
        this->kernel_pml4 = static_cast<pte_t*>(pml4->data[0]);

        for (size_t n = 0; n < 4 * GB; n += PAGE_SIZE)
        {
            this->map(n, n, 0x3);
            this->map(n, to_higher_half(n, DATA), 0x3);
        }

        for (size_t n = 0; n < 0x80000000; n += PAGE_SIZE)
        {
            this->map(n, to_higher_half(n, CODE), 0x3);
        }
        
        asm volatile("mov %0, %%cr3\n" :: "r"(this->kernel_pml4));
    }

    void VirtualMemoryManager::map(phys_t physical_addr, virt_t virtual_addr, int access_flags)
    {
        auto idx4 = this->get_index(virtual_addr, 4);
        auto idx3 = this->get_index(virtual_addr, 3);
        auto idx2 = this->get_index(virtual_addr, 2);
        auto idx1 = this->get_index(virtual_addr, 1);

        if (!(this->kernel_pml4[idx4] & 1)) {
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