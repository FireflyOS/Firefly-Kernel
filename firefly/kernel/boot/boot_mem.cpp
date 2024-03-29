#include "firefly/intel64/cpu/cpu.hpp"
#include "firefly/intel64/page_flags.hpp"
#include "firefly/intel64/page_permissions.hpp"
#include "firefly/memory-manager/primary/buddy.hpp"
#include "firefly/panic.hpp"
#include "libk++/align.h"

namespace firefly::boot {
using namespace kernel;
using namespace core::paging;

BuddyAllocator pageAllocator{};

// Index within the Pml (0-511)
inline int64_t getPmlOffset(const uint64_t virtual_addr, const int depth) {
    // Dissect the virtual address by retrieving 9 bits starting at `depth - 1`
    return (virtual_addr >> (PAGE_SHIFT + (9 * (depth - 1)))) & 0x1FF;  // We subtract 1 from idx so that we don't have to input idx 0-3, but rather 1-4
}

inline uint64_t *allocatePageTable(uint64_t size = PageSize::Size4K) {
    uint64_t *ptr = pageAllocator.alloc(4096).unpack();

    if (!ptr)
        firefly::panic("Unable to allocate memory for a page-table");

    return ptr;
}

void traverse_page_tables(const uint64_t virtual_addr, const uint64_t physical_addr, const int access_flags, uint64_t *pml_ptr, const PageSize page_size = PageSize::Size4K) {
    auto idx4 = getPmlOffset(virtual_addr, 4);
    auto idx3 = getPmlOffset(virtual_addr, 3);
    auto idx2 = getPmlOffset(virtual_addr, 2);
    auto idx1 = getPmlOffset(virtual_addr, 1);

    if (!(pml_ptr[idx4] & 1)) {
        uint64_t *ptr = allocatePageTable();
        pml_ptr[idx4] = reinterpret_cast<uint64_t>(ptr);
        pml_ptr[idx4] |= access_flags;
    }
    auto pml3 = reinterpret_cast<uint64_t *>(pml_ptr[idx4] & ~(511));
    if (page_size == PageSize::Size1G) {
        pml3[idx3] = (physical_addr | access_flags | BIT(7));
        return;
    }
    if (!(pml3[idx3] & 1)) {
        const auto ptr = allocatePageTable();
        pml3[idx3] = reinterpret_cast<uint64_t>(ptr);
        pml3[idx3] |= access_flags;
    }
    auto pml2 = reinterpret_cast<uint64_t *>(pml3[idx3] & ~(511));
    if (page_size == PageSize::Size2M) {
        pml2[idx2] = (physical_addr | access_flags | BIT(7));
        return;
    }

    if ((pml2[idx2] & 1) == 0) {
        const auto ptr = allocatePageTable();
        pml2[idx2] = reinterpret_cast<uint64_t>(ptr);
        pml2[idx2] |= access_flags;
    }

    const auto pml1 = reinterpret_cast<uint64_t *>(pml2[idx2] & ~(511));
    pml1[idx1] = (physical_addr | access_flags);
}

void map(uint64_t virtual_addr, uint64_t physical_addr, AccessFlags access_flags, const uint64_t *pml_ptr, const PageSize page_size) {
    traverse_page_tables(virtual_addr, physical_addr, static_cast<const int>(access_flags), const_cast<uint64_t *>(pml_ptr), page_size);
    asm volatile("invlpg %0" ::"m"(virtual_addr)
                 : "memory");
}

// Map some memory for the pagelist (page.hpp)
// This is required for the Physical memory manager to function.
void bootMapExtraRegion(limine_memmap_response *mmap) {
    constexpr int required_size = 4;

    for (uint64_t i = 0; i < mmap->entry_count; i++) {
        auto e = mmap->entries[i];
        if (e->type != LIMINE_MEMMAP_USABLE || e->length < MiB(required_size))
            continue;

        pageAllocator.init(BuddyAllocator::AddressType(e->base), 22);
        e->base = libkern::align_up4k(e->base + MiB(required_size));
        e->length -= libkern::align_down4k(MiB(required_size));
        break;
    }

    // clang-format off
    auto cr3{ 0ul };
    asm volatile("mov %%cr3, %0": "=r"(cr3));
    // clang-format on

    if (cpuHugePages()) {
        map(AddressLayout::PageData, 0, AccessFlags::ReadWrite, reinterpret_cast<const uint64_t *>(cr3), PageSize::Size1G);
    } else {
        for (uint32_t i = 0; i < GiB(1); i += PageSize::Size2M) {
            map(i + AddressLayout::PageData, i, AccessFlags::ReadWrite, reinterpret_cast<const uint64_t *>(cr3), PageSize::Size2M);
        }
    }
}


}  // namespace firefly::boot
