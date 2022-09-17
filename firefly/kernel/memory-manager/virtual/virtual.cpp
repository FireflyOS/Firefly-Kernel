#include "firefly/memory-manager/virtual/virtual.hpp"

#include "firefly/compiler/compiler.hpp"
#include "firefly/console/console.hpp"
#include "firefly/limine.hpp"
#include "firefly/logger.hpp"
#include "firefly/memory-manager/primary/primary_phys.hpp"
#include "firefly/panic.hpp"
#include "libk++/bits.h"
#include "firefly/memory-manager/primary/buddy.hpp"

namespace firefly::kernel::mm {

USED struct limine_kernel_address_request kernel_address {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST, .revision = 0, .response = nullptr
};

frg::manual_box<kernelPageSpace> kPageSpaceSingleton{};

kernelPageSpace &kernelPageSpace::accessor() {
    return *kPageSpaceSingleton;
}

void kernelPageSpace::init() {
    auto pml4 = static_cast<T *>(Physical::must_allocate());
    kPageSpaceSingleton.initialize(pml4);

    kPageSpaceSingleton.get()->mapRange(PAGE_SIZE, buddy.get_highest_address(), AccessFlags::ReadWrite, AddressLayout::Low);
    kPageSpaceSingleton.get()->mapRange(0, GiB(4), AccessFlags::ReadWrite, AddressLayout::High);

    for (size_t i = kernel_address.response->physical_base, j = 0; i < kernel_address.response->physical_base + GiB(1); i += SIZE_4KB, j += SIZE_4KB)
		kPageSpaceSingleton.get()->map(j + kernel_address.response->virtual_base, i, AccessFlags::ReadWrite, SIZE_4KB);
    
	kPageSpaceSingleton.get()->mapRange(0, GiB(1), AccessFlags::ReadWrite, AddressLayout::PageData, SIZE_2MB);
    kPageSpaceSingleton.get()->loadAddressSpace();

    ConsoleLogger::log() << "vmm: Initialized" << logger::endl;
}
}  // namespace firefly::kernel::mm
