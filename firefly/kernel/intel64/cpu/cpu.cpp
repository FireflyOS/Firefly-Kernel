#include "firefly/intel64/cpu/cpu.hpp"

#include "firefly/intel64/cpu/ap/ap.hpp"
#include "firefly/intel64/cpu/apic/apic.hpp"
#include "firefly/intel64/int/interrupt.hpp"
#include "firefly/memory-manager/allocator.hpp"
#include "firefly/memory-manager/secondary/heap.hpp"
#include "frg/manual_box.hpp"
#include "frg/spinlock.hpp"
#include "frg/vector.hpp"

namespace firefly::kernel {
namespace {
frg::vector<CpuData *, Allocator> allCpuContexts;
frg::ticket_spinlock allCpuContextsLock;
}  // namespace

void initializeBootProccessor(uint64_t stack) {
    initializeThisCpu(stack);
}

void initializeApplicationProcessor(uint64_t stack) {
    initializeThisCpu(stack);
}

CpuData *getCpuData(size_t k) {
    return allCpuContexts[k];
}

CpuData *getLocalCpuData() {
    AssemblyCpuData *cpu_data;
    asm volatile("mov %%gs:0, %0"
                 : "=r"(cpu_data));
    return reinterpret_cast<CpuData *>(cpu_data);
}

void initializeThisCpu(uint64_t stack) {
    auto cpuData = new (mm::heap->allocate(sizeof(CpuData))) CpuData;
    cpuData->selfPointer = cpuData;

    core::gdt::init(cpuData->gdt);
    core::tss::init(cpuData, stack);

    // Write cpu data base to GS base
    wrmsr(MSR::GsBase, reinterpret_cast<uint64_t>(cpuData));

    firefly::kernel::core::interrupt::init();
    apic::Apic::init();

    cpuData->cpuIndex = apic::Apic::accessor().apicId();
    allCpuContexts.push(cpuData);

    logLine << "Initialized CPU " << fmt::dec << cpuData->cpuIndex << '\n'
            << fmt::endl;
}

}  // namespace firefly::kernel
