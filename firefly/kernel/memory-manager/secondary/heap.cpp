#include "firefly/memory-manager/secondary/heap.hpp"

#include <frg/list.hpp>

#include "cstdlib/cmath.h"
#include "firefly/logger.hpp"
#include "firefly/memory-manager/primary/buddy.hpp"
#include "firefly/memory-manager/secondary/slab/slab.hpp"
#include "firefly/memory-manager/virtual/virtual.hpp"

namespace firefly::kernel::mm {

namespace {
static constexpr bool sanityCheckVmBackingAllocator{ false };
}

BuddyAllocator vm_buddy;

// Todo:
// The slab allocator needs to resize itself (i.e. add or remove(free) slabs).
// (Useful when the system is low on memory)
struct VmBackingAllocator {
    VirtualAddress allocate(int size) {
        auto ptr = vm_buddy.alloc(size).unpack();

        if constexpr (sanityCheckVmBackingAllocator)
            if (ptr == nullptr)
                panic("vm_buddy returned a null-pointer, this should never happen!");

        return VirtualAddress(ptr);
    }
};

struct LockingMechanism {
    void lock() {
    }
    void unlock() {
    }
};

using cacheType = slabCache<VmBackingAllocator, LockingMechanism>;
frg::array<cacheType, 12> kernelAllocatorCaches = {};

constinit frg::manual_box<kernelHeap> heap;

void kernelHeap::init() {
    // Map kernel heap with physical memory to back it up.
    for (int i = 0; i < GiB(1); i += PageSize::Size4K) {
        auto phys = reinterpret_cast<uintptr_t>(Physical::must_allocate());
        kernelPageSpace::accessor().map(i + AddressLayout::SlabHeap, phys, AccessFlags::ReadWrite, PageSize::Size4K);
    }

    // Initialize separate buddy instance to manage heap memory (VmBackingAllocator)
    vm_buddy.init(reinterpret_cast<uint64_t*>(AddressLayout::SlabHeap), BuddyAllocator::largest_allowed_order);

    heap.initialize();
    for (size_t i = 0, j = 1; i < kernelAllocatorCaches.size(); i++, j++)
        kernelAllocatorCaches[i].initialize(1 << j, "heap");
}

VirtualAddress kernelHeap::allocate(size_t size) const {
    assert_truth(size >= 2 && size <= 4096 && "Invalid allocation size");

    auto& cache = kernelAllocatorCaches[log2(size) - 1];
    return cache.allocate();
}

void kernelHeap::deallocate(VirtualAddress ptr) const {
    if (ptr == nullptr)
        return;

    auto aligned_address = (reinterpret_cast<uintptr_t>(ptr) >> PAGE_SHIFT) << PAGE_SHIFT;
    auto size = pagelist.phys_to_page(aligned_address - AddressLayout::SlabHeap)->slab_size;
    auto& cache = kernelAllocatorCaches[log2(size) - 1];
    cache.deallocate(ptr);
}

}  // namespace firefly::kernel::mm