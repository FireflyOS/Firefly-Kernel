#include "firefly/intel64/cpu/ap/ap.hpp"
#include "firefly/intel64/cpu/cpu.hpp"
#include "firefly/limine.hpp"
#include "firefly/drivers/ports.hpp"
#include "firefly/memory-manager/mm.hpp"
#include "firefly/memory-manager/primary/primary_phys.hpp"
#include "firefly/logger.hpp"

#include <cstddef>
#include <cstdint>


namespace firefly::kernel::applicationProcessor {
// Contains the BSP ID
volatile struct limine_smp_request smp_request {
    .id = LIMINE_SMP_REQUEST, .revision = 0, .response = nullptr
};

void smp_main(struct limine_smp_info* info) {
	initializeThisCpu(info->extra_argument);
	asm volatile ("cli; hlt");
}

void startAllCores() {
	struct limine_smp_response *smp = smp_request.response;

	for (uint64_t i = 0; i < smp->cpu_count; i++) {
		auto cpu = smp->cpus[i];
		cpu->extra_argument = reinterpret_cast<uint64_t>(mm::Physical::allocate(PageSize::Size4K * 2));
		cpu->goto_address = &smp_main;
	}
}

}  // namespace firefly::kernel::applicationProcessor
