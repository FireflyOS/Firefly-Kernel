#include "firefly/intel64/paging.hpp"

#include "firefly/memory-manager/primary/primary_phys.hpp"
#include "firefly/panic.hpp"

void *operator new(uint64_t size) {
    return firefly::kernel::mm::Physical::must_allocate(size);
}

namespace firefly::kernel::core::paging {

void invalidatePage(const VirtualAddress page) {
    asm volatile("invlpg %0" ::"m"(page)
                 : "memory");
}

void invalidatePage(const uint64_t page) {
    invalidatePage(reinterpret_cast<const VirtualAddress>(page));
}

inline int64_t get_index(const uint64_t virtual_addr, const int idx) {
    // Dissect the virtual address by retrieving 9 bits starting at `idx - 1`
    return (virtual_addr >> (PAGE_SHIFT + (9 * (idx - 1)))) & 0x1FF;  // We subtract 1 from idx so that we don't have to input idx 0-3, but rather 1-4
}

void traverse_page_tables(const uint64_t virtual_addr, const uint64_t physical_addr, const int access_flags, uint64_t *pml_ptr) {
    auto idx4 = get_index(virtual_addr, 4);
    auto idx3 = get_index(virtual_addr, 3);
    auto idx2 = get_index(virtual_addr, 2);
    auto idx1 = get_index(virtual_addr, 1);

    if (!(pml_ptr[idx4] & 1)) {
        const auto ptr = new uint64_t(PAGE_SIZE);
        pml_ptr[idx4] = reinterpret_cast<uint64_t>(ptr);
        pml_ptr[idx4] |= access_flags;
    }
    auto pml3 = reinterpret_cast<uint64_t *>(pml_ptr[idx4] & ~(511));

    if (!(pml3[idx3] & 1)) {
        const auto ptr = new uint64_t(PAGE_SIZE);
        pml3[idx3] = reinterpret_cast<uint64_t>(ptr);
        pml3[idx3] |= access_flags;
    }
    auto pml2 = reinterpret_cast<uint64_t *>(pml3[idx3] & ~(511));

    if ((pml2[idx2] & 1) == 0) {
        const auto ptr = new uint64_t(PAGE_SIZE);
        pml2[idx2] = reinterpret_cast<uint64_t>(ptr);
        pml2[idx2] |= access_flags;
    }

    const auto pml1 = reinterpret_cast<uint64_t *>(pml2[idx2] & ~(511));
    pml1[idx1] = (physical_addr | access_flags);
}

void map(uint64_t physical_addr, uint64_t virtual_addr, AccessFlags access_flags, const uint64_t *pml_ptr) {
    traverse_page_tables(physical_addr, virtual_addr, static_cast<const int>(access_flags), const_cast<uint64_t *>(pml_ptr));
    invalidatePage(reinterpret_cast<VirtualAddress>(virtual_addr));
}

}  // namespace firefly::kernel::core::paging