#include "firefly/memory-manager/virtual/virtual.hpp"

#include "firefly/console/console.hpp"
#include "firefly/logger.hpp"
#include "firefly/memory-manager/primary/primary_phys.hpp"
#include "firefly/panic.hpp"
#include "libk++/bits.h"

namespace firefly::kernel::mm {

frg::manual_box<kernelPageSpace> kPageSpaceSingleton{};

kernelPageSpace &kernelPageSpace::accessor() {
    return *kPageSpaceSingleton;
}

void kernelPageSpace::init() {
    auto pml4 = static_cast<T *>(Physical::must_allocate());
    kPageSpaceSingleton.initialize(pml4);

    kPageSpaceSingleton.get()->mapRange(PAGE_SIZE, GiB(4), AccessFlags::ReadWrite, AddressLayout::Low);
    kPageSpaceSingleton.get()->mapRange(0, GiB(2), AccessFlags::ReadWrite, AddressLayout::High);
    kPageSpaceSingleton.get()->mapRange(0, GiB(2), AccessFlags::Readonly, AddressLayout::Code);
    kPageSpaceSingleton.get()->mapRange(0, GiB(1), AccessFlags::ReadWrite, AddressLayout::PageData);
    kPageSpaceSingleton.get()->loadAddressSpace();

    info_logger << "vmm: Initialized" << logger::endl;
}
}  // namespace firefly::kernel::mm