#include "firefly/intel64/cpu/cpu.hpp"

#include "firefly/intel64/cpu/apic/apic.hpp"
#include "firefly/intel64/cpu/ap/ap.hpp"
#include "firefly/intel64/int/interrupt.hpp"

// Note: This is purposely bare-bones and lacking a
// proper "per cpu" structure (i.e. gsbase).
// There is no multi-core yet, but I'd prefer to structure
// everything in a way so that it's easy to get it up and running now rather than later.
namespace firefly::kernel {
CpuData temporaryCpuInstance;

void initializeThisCpu(uint64_t stack) {
    core::gdt::init(temporaryCpuInstance.gdt);
    core::tss::init(stack);
    firefly::kernel::core::interrupt::init();
    applicationProcessor::startAllCores();
    apic::init();
}

CpuData &thisCpu() {
    return temporaryCpuInstance;
}
}  // namespace firefly::kernel
