#include "firefly/memory-manager/secondary/heap.hpp"

#include <frg/list.hpp>

#include "cstdlib/cmath.h"
#include "firefly/logger.hpp"
#include "firefly/memory-manager/secondary/slab/slab.hpp"
#include "firefly/memory-manager/virtual/virtual.hpp"

namespace firefly::kernel::mm {
// Todo: This needs to map, unmap, poison and offset memory.
class VmBackingAllocator {
public:
    void* allocate(int s) {
        return firefly::kernel::mm::Physical::allocate(s);
    }
};

class LockingMechanism {
public:
    void lock() {
    }
    void unlock() {
    }
};

using cacheType = slabCache<VmBackingAllocator, LockingMechanism>;
frg::array<cacheType, 12> kernelAllocatorCaches = {};

constinit frg::manual_box<kernelHeap> heap;

void kernelHeap::init() {
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
    auto size = pagelist.phys_to_page(aligned_address)->slab_size;
    auto& cache = kernelAllocatorCaches[log2(size) - 1];
    cache.deallocate(ptr);
}

}  // namespace firefly::kernel::mm