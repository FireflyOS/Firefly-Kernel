#include "firefly/memory-manager/primary/primary_phys.hpp"

#include "firefly/memory-manager/page.hpp"
#include "firefly/memory-manager/primary/buddy.hpp"
#include "frg/spinlock.hpp"

// TODO: Use constinit
Pagelist pagelist;
BuddyManager buddy;
frg::ticket_spinlock buddyLock = frg::ticket_spinlock();

namespace firefly::kernel::mm::Physical {


void init(limine_memmap_response *mmap) {
    buddy.init(mmap);
    pagelist.init(mmap);
    logLine << "pmm: Initialized" << fmt::endl;
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
    buddy.free(static_cast<uint64_t *>(ptr));
}
}  // namespace firefly::kernel::mm::Physical
