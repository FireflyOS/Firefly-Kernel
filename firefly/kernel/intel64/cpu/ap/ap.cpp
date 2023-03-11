#include "firefly/intel64/cpu/ap/ap.hpp"

#include <cstddef>
#include <cstdint>

#include "firefly/drivers/ports.hpp"
#include "firefly/intel64/cpu/apic/apic.hpp"
#include "firefly/intel64/cpu/cpu.hpp"
#include "firefly/limine.hpp"
#include "firefly/logger.hpp"
#include "firefly/memory-manager/mm.hpp"
#include "firefly/memory-manager/primary/primary_phys.hpp"
#include "firefly/memory-manager/virtual/virtual.hpp"
#include "frg/spinlock.hpp"


namespace firefly::kernel::applicationProcessor {
volatile struct limine_smp_request smp_request {
    .id = LIMINE_SMP_REQUEST, .revision = 0, .response = nullptr
};

static frg::ticket_spinlock initLock;

void smp_main(struct limine_smp_info* info) {
    auto stack = reinterpret_cast<uintptr_t>(mm::Physical::must_allocate(PageSize::Size4K * 2));

    // clang-format off
    asm volatile("mov %0, %%rsp" ::"r"(stack+PageSize::Size4K * 2) : "memory");
    asm volatile("mov %0, %%rbp" ::"r"(stack): "memory");
    // clang-format on

    mm::kernelPageSpace::accessor().setCR3();
    initializeApplicationProcessor(stack);
    initLock.unlock();

    for (;;)
        asm volatile("cli; hlt");
}

void startAllCores() {
    struct limine_smp_response* smp = smp_request.response;

    for (uint64_t i = 0; i < smp->cpu_count; i++) {
        auto cpu = smp->cpus[i];
        cpu->goto_address = &smp_main;
        initLock.lock();
    }

    apic::IOApic::initAll();
}

}  // namespace firefly::kernel::applicationProcessor
