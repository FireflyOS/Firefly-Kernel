#include "firefly/memory-manager/primary/primary_phys.hpp"

#include "firefly/memory-manager/page.hpp"
#include "firefly/memory-manager/primary/buddy.hpp"

// TODO: Use constinit
Pagelist pagelist;
BuddyManager buddy;

namespace firefly::kernel::mm::Physical {


void init(limine_memmap_response *mmap) {
    buddy.init(mmap);
    pagelist.init(mmap);
    info_logger << "pmm: Initialized" << logger::endl;
}

PhysicalAddress allocate(uint64_t size, FillMode fill) {
    (void)fill;
    return buddy.alloc(size);
}

PhysicalAddress must_allocate(uint64_t size, FillMode fill) {
    (void)fill;
    return buddy.must_alloc(size);
}

void deallocate(PhysicalAddress ptr) {
    buddy.free(static_cast<BuddyAllocator::AddressType>(ptr));
}
}  // namespace firefly::kernel::mm::Physical