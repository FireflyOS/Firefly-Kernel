#include <stddef.h>
#include <stdint.h>

#include "firefly/console/console.hpp"
#include "firefly/intel64/acpi/acpi.hpp"
#include "firefly/intel64/cpu/cpu.hpp"
#include "firefly/kernel.hpp"
#include "firefly/limine.hpp"
#include "firefly/logger.hpp"
#include "firefly/memory-manager/primary/primary_phys.hpp"
#include "firefly/memory-manager/virtual/virtual.hpp"

alignas(uint16_t) static uint8_t stack[PageSize::Size4K * 2] = { 0 };

USED struct limine_memmap_request memmap {
    .id = LIMINE_MEMMAP_REQUEST, .revision = 0, .response = nullptr
};

void bootloaderServicesInit() {
    using namespace firefly::kernel;
    const auto verify = [](auto tag) {
        if (unlikely(tag == NULL)) {
            for (;;)
                asm("hlt");
        }
        return tag;
    };

    auto tagmem = verify(memmap.response);

    ConsoleLogger::init();
    SerialLogger::init();

    core::paging::bootMapExtraRegion(tagmem);
    mm::Physical::init(tagmem);
    mm::kernelPageSpace::init();

    core::acpi::Acpi::init();
    console::init();
    log_core_firefly_contributors();
}

extern "C" [[noreturn]] [[gnu::naked]] void kernel_init() {
    // clang-format off
    asm volatile("mov %0, %%rsp" ::"r"(((uintptr_t)stack) + (PAGE_SIZE * 2)) : "memory");
    asm volatile("mov %0, %%rbp" ::"r"((uintptr_t)stack): "memory");
    // clang-format on

    bootloaderServicesInit();
    firefly::kernel::initializeThisCpu(reinterpret_cast<uint64_t>(stack));

    firefly::kernel::kernel_main();
    __builtin_unreachable();
}
