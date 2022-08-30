#include "firefly/init/init.hpp"

#include <stddef.h>
#include <stdint.h>

#include "firefly/console/console.hpp"
#include "firefly/intel64/cpu/cpu.hpp"
#include "firefly/intel64/int/interrupt.hpp"
#include "firefly/kernel.hpp"
#include "firefly/limine.hpp"
#include "firefly/memory-manager/primary/primary_phys.hpp"
#include "firefly/memory-manager/virtual/virtual.hpp"

__attribute__((section(".bss.bsp_stack")))
constinit uint8_t stack[PAGE_SIZE * 2] = { 0 };

USED struct limine_memmap_request memmap {
    .id = LIMINE_MEMMAP_REQUEST, .revision = 0, .response = nullptr
};

void bootloader_services_init() {
    using namespace firefly::kernel;
    const auto verify = [](auto tag) {
        if (unlikely(tag == NULL)) {
            for (;;)
                asm("hlt");
        }
        return tag;
    };

    auto tagmem = verify(memmap.response);

    core::paging::bootMapExtraRegion(tagmem);
    mm::Physical::init(tagmem);
    // mm::kernelPageSpace::init();

    console::init();
    console::write("Hello, world!");
}

extern "C" [[noreturn]] void kernel_init() {
    firefly::kernel::initializeThisCpu(reinterpret_cast<uint64_t>(stack));
    firefly::kernel::core::interrupt::init();

    bootloader_services_init();

    firefly::kernel::kernel_main();
    __builtin_unreachable();
}
