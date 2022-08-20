#include "firefly/memory-manager/primary/primary_phys.hpp"

#include "firefly/memory-manager/page.hpp"
#include "firefly/memory-manager/primary/buddy.hpp"

Pagelist pagelist;

namespace firefly::kernel::mm::pmm {

static BuddyManager buddy;

void init(stivale2_struct_tag_memmap *mmap) {
    buddy.init(mmap);
    pagelist.init(mmap);
    info_logger << "pmm: Initialized" << logger::endl;
}

PhysicalAddress allocate(uint64_t size, FillMode fill) {
    return buddy.alloc(size, fill);
}

void deallocate(PhysicalAddress ptr) {
    buddy.free(static_cast<BuddyAllocator::AddressType>(ptr));
}
}  // namespace firefly::kernel::mm::pmm