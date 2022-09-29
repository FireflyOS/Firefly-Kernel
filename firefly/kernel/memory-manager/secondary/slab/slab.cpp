#include "firefly/memory-manager/secondary/slab/slab.hpp"

#include <frg/array.hpp>

#include "firefly/logger.hpp"

namespace firefly::kernel::mm::secondary {

class VmBackingAllocator {

};

frg::array<slabCache<VmBackingAllocator, int>, 8> ss {

};



}  // namespace firefly::kernel::mm::secondary