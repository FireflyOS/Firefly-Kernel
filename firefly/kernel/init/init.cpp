#include <stddef.h>
#include <stdint.h>

#include "firefly/boot/boot_mem.hpp"
#include "firefly/console/console.hpp"
#include "firefly/intel64/acpi/acpi.hpp"
#include "firefly/intel64/cpu/ap/ap.hpp"
#include "firefly/intel64/cpu/cpu.hpp"
#include "firefly/kernel.hpp"
#include "firefly/limine.hpp"
#include "firefly/logger.hpp"
#include "firefly/memory-manager/primary/primary_phys.hpp"
#include "firefly/memory-manager/stack.hpp"
#include "firefly/memory-manager/virtual/virtual.hpp"
#include "firefly/timer/timer.hpp"
#include "firefly/trace/sanitizer/kasan.hpp"

constinit frg::manual_box<kernelStack> kStack;

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

    firefly::boot::bootMapExtraRegion(tagmem);

    mm::Physical::init(tagmem);
    mm::kernelPageSpace::init();
    mm::kernelHeap::init();

    core::acpi::Acpi::init();
    console::init();

    kasan::init();
}

extern "C" [[noreturn]] [[gnu::naked]] void kernel_init() {
    kStack.initialize();
    Stack::stack stk{
        .rbp = reinterpret_cast<uintptr_t>(stack),
        .rsp = reinterpret_cast<uintptr_t>(stack + (PageSize::Size4K * 2))
    };
    kStack->add(stk);

    // clang-format off
    asm volatile("mov %0, %%rbp" :: "r"(stk.rbp) : "memory");
    asm volatile("mov %0, %%rsp" :: "r"(stk.rsp) : "memory");
    // clang-format off

    bootloaderServicesInit();

    firefly::kernel::initializeBootProccessor(reinterpret_cast<uint64_t>(stack));
    firefly::kernel::applicationProcessor::startAllCores();
    firefly::kernel::timer::init();

    firefly::kernel::kernel_main();
    __builtin_unreachable();
}
