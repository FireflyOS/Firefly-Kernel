#include "firefly/intel64/cpu/ap/ap.hpp"

#include <cstddef>
#include <cstdint>

#include "firefly/drivers/ports.hpp"
#include "firefly/intel64/cpu/cpu.hpp"
#include "firefly/limine.hpp"
#include "firefly/logger.hpp"
#include "firefly/memory-manager/mm.hpp"
#include "firefly/memory-manager/primary/primary_phys.hpp"
#include "firefly/memory-manager/virtual/virtual.hpp"


namespace firefly::kernel::applicationProcessor {
volatile struct limine_smp_request smp_request {
    .id = LIMINE_SMP_REQUEST, .revision = 0, .response = nullptr
};

void smp_main(struct limine_smp_info* info) {
    // clang-format off
    asm volatile("mov %0, %%rsp" ::"r"((uintptr_t)info->extra_argument) : "memory");
    asm volatile("mov %0, %%rbp" ::"r"((uintptr_t)info->extra_argument): "memory");
    // clang-format on

    mm::kernelPageSpace::accessor().set_AP_CR3();
    initializeThisCpu(info->extra_argument);

    asm volatile("cli; hlt");
}

void startAllCores() {
    struct limine_smp_response* smp = smp_request.response;

    for (uint64_t i = 0; i < smp->cpu_count; i++) {
        auto cpu = smp->cpus[i];
        cpu->extra_argument = reinterpret_cast<uint64_t>(mm::Physical::allocate(PageSize::Size4K * 4));
        cpu->goto_address = &smp_main;
    }
}

}  // namespace firefly::kernel::applicationProcessor
