#include "firefly/intel64/paging.hpp"

#include "firefly/compiler/compiler.hpp"
#include "firefly/intel64/cpu/cpu.hpp"
#include "firefly/memory-manager/primary/page_frame.hpp"
#include "firefly/memory-manager/primary/primary_phys.hpp"
#include "firefly/panic.hpp"
#include "libk++/bits.h"

namespace firefly::kernel::core::paging {

void invalidatePage(const VirtualAddress page) {
    asm volatile("invlpg %0" ::"m"(page)
                 : "memory");
}

void invalidatePage(const uint64_t page) {
    invalidatePage(reinterpret_cast<const VirtualAddress>(page));
}

// Index within the Pml (0-511)
inline int64_t getPmlOffset(const uint64_t virtual_addr, const int depth) {
    // Dissect the virtual address by retrieving 9 bits starting at `depth - 1`
    return (virtual_addr >> (PAGE_SHIFT + (9 * (depth - 1)))) & 0x1FF;  // We subtract 1 from idx so that we don't have to input idx 0-3, but rather 1-4
}

// Number of translation levels
using PageDepth = int;

// Returns the number of translation levels
// -> Pml3, Pml2, Pml1
PageDepth depthOf(const PageSize page_size) {
    switch (page_size) {
        case PageSize::Size1G:
            return 3;

        case PageSize::Size2M:
            return 2;

        // 4k
        default:
            return 1;
    }

    __builtin_unreachable();
}

void traversePageTables(const uint64_t virtual_addr, const uint64_t physical_addr, AccessFlags access_flags, Pml *pml, const PageSize page_size = PageSize::Size4K) {
    int depth = 4;
    const int target_depth = depthOf(page_size);

    // Stop at pml1
    while (depth != target_depth) {
        pml = pml->next(getPmlOffset(virtual_addr, depth), access_flags, page_size);
        depth--;
    }

    // lowest_pml_index changes depending on the number of translation levels we use.
    // This can vary for 4K, 2M and 1G pages
    auto lowest_pml_index = getPmlOffset(virtual_addr, depth);

    // Set the PS bit
    int flags = static_cast<int>(access_flags);
    if (lowest_pml_index != 1) flags |= BIT(7);

    pml->entries[lowest_pml_index].create(physical_addr, flags, page_size);
}

void map(uint64_t virtual_addr, uint64_t physical_addr, AccessFlags access_flags, Pml *root, const PageSize page_size) {
    traversePageTables(virtual_addr, physical_addr, access_flags, root, page_size);
    invalidatePage(virtual_addr);
}


}  // namespace firefly::kernel::core::paging
