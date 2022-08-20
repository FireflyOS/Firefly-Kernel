#include "firefly/memory-manager/virtual/virtual.hpp"

#include "firefly/console/stivale2-term.hpp"
#include "firefly/logger.hpp"
#include "firefly/memory-manager/primary/primary_phys.hpp"
#include "firefly/memory-manager/relocation.hpp"
#include "firefly/panic.hpp"

namespace firefly::kernel::mm {
using namespace firefly::mm::relocation::conversion;
using namespace mm;

constexpr size_t GB = 0x40000000UL;
constexpr size_t PAGE_SIZE = 4096;

VirtualMemoryManager::VirtualMemoryManager(bool initial_mapping, stivale2_struct_tag_memmap *mmap) {
    if (initial_mapping)
        this->configure_initial_kernel_mapping(mmap);
}

void VirtualMemoryManager::configure_initial_kernel_mapping(stivale2_struct_tag_memmap *mmap) {
    auto pml4 = pmm::allocate();
    if (pml4 == nullptr) panic("Failed to allocate memory for the kernel pml4");
    this->kernel_pml4 = static_cast<pte_t *>(pml4);

    for (size_t n = PAGE_SIZE; n < GB * 4; n += PAGE_SIZE) {
        this->map(n, n, 0x3, kernel_pml4);
    }
    
    for (auto i = 0ul; i < mmap->entries; i++) {
        auto entry = mmap->memmap[i];
        if (entry.type == STIVALE2_MMAP_USABLE) {
            auto base = entry.base;
            auto top = base + entry.length;
            for (; base != top; base += PAGE_SIZE) {
                this->map(base, base, 0x3, kernel_pml4);
            }
        }
    }

    for (size_t n = 0; n < 2 * GB; n += PAGE_SIZE) {
        this->map(n, to_higher_half(n, DATA), 0x3, kernel_pml4);
    }

    for (size_t base = 0; base < 2 * GB; base += PAGE_SIZE) {
        this->map(base, to_higher_half(base, CODE), 0x3, kernel_pml4);
    }

    asm volatile("mov %0, %%cr3\n" ::"r"(this->kernel_pml4));
    info_logger << "vmm: Initialized" << logger::endl;
}

walk_t VirtualMemoryManager::walk(virt_t virtual_addr, pte_t *pml_ptr, uint64_t access_flags) {
    auto idx4 = this->get_index(virtual_addr, 4);
    auto idx3 = this->get_index(virtual_addr, 3);
    auto idx2 = this->get_index(virtual_addr, 2);
    auto idx1 = this->get_index(virtual_addr, 1);

    if (!(pml_ptr[idx4] & 1)) {
        auto ptr = pmm::allocate();
        if (ptr == nullptr) panic("OOM");
        this->kernel_pml4[idx4] = reinterpret_cast<pte_t>(ptr);
        this->kernel_pml4[idx4] |= access_flags;
    }
    auto pml3 = (pte_t *)(this->kernel_pml4[idx4] & ~(511));

    if (!(pml3[idx3] & 1)) {
        auto ptr = pmm::allocate();
        if (ptr == nullptr) panic("OOM");
        pml3[idx3] = reinterpret_cast<pte_t>(ptr);
        pml3[idx3] |= access_flags;
    }
    auto pml2 = (pte_t *)(pml3[idx3] & ~(511));

    if ((pml2[idx2] & 1) == 0) {
        auto ptr = pmm::allocate();
        if (ptr == nullptr) panic("OOM");
        pml2[idx2] = reinterpret_cast<pte_t>(ptr);
        pml2[idx2] |= access_flags;
    }
    return {
        .idx = idx1,
        .pml1 = (pte_t *)(pml2[idx2] & ~(511))
    };
}

void VirtualMemoryManager::map(phys_t physical_addr, virt_t virtual_addr, uint64_t access_flags, pte_t *pml_ptr) {
    auto table_walk_result = walk(virtual_addr, pml_ptr, access_flags);
    table_walk_result.pml1[table_walk_result.idx] = (physical_addr | access_flags);
    asm volatile("invlpg %0" ::"m"(virtual_addr)
                 : "memory");
}
}  // namespace firefly::kernel::mm