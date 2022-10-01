#include "firefly/memory-manager/secondary/slab/slab.hpp"

#include <frg/array.hpp>

#include "firefly/logger.hpp"

namespace firefly::kernel::mm {

// Todo: This needs to map, unmap, poison and offset memory.
class VmBackingAllocator {
    void *allocate(int s) {
        return nullptr;
    }
};

class LockingMechanism {
public:
    void lock() {
    }
    void unlock() {
    }
};

// List of slab caches
using slabCaches = slabCache<VmBackingAllocator, LockingMechanism>;
frg::intrusive_list<slabCaches, frg::locate_member<slabCaches, frg::default_list_hook<slabCaches>, &slabCaches::node>> kernelAllocatorCaches;

// Here we would have some kind of kernel allocator singleton which uses the 'kernelAllocatorCaches`

}  // namespace firefly::kernel::mm