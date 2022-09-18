#include "firefly/memory-manager/virtual/virtual.hpp"

#include "firefly/compiler/compiler.hpp"
#include "firefly/console/console.hpp"
#include "firefly/intel64/cpu/cpu.hpp"
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

    kPageSpaceSingleton.get()->mapRange(PageSize::Size4K, buddy.get_highest_address(), AccessFlags::ReadWrite, AddressLayout::Low);

    if (cpuHugePages()) {
    	kPageSpaceSingleton.get()->mapRange(0, GiB(4), AccessFlags::ReadWrite, AddressLayout::High, PageSize::Size1G);
    	kPageSpaceSingleton.get()->map(AddressLayout::PageData, 0, AccessFlags::ReadWrite, PageSize::Size1G);
    } else {    
    	kPageSpaceSingleton.get()->mapRange(0, GiB(4), AccessFlags::ReadWrite, AddressLayout::High, PageSize::Size2M);
    	kPageSpaceSingleton.get()->mapRange(0, GiB(1), AccessFlags::ReadWrite, AddressLayout::PageData, PageSize::Size2M);
    }

    for (size_t i = kernel_address.response->physical_base, j = 0; i < kernel_address.response->physical_base + GiB(1); i += PageSize::Size4K, j += PageSize::Size4K)
		kPageSpaceSingleton.get()->map(j + kernel_address.response->virtual_base, i, AccessFlags::ReadWrite, PageSize::Size4K);
   
    kPageSpaceSingleton.get()->loadAddressSpace();

    ConsoleLogger::log() << "vmm: Initialized" << logger::endl;
}
}  // namespace firefly::kernel::mm
