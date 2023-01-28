#include "firefly/intel64/cpu/cpu.hpp"

#include "firefly/intel64/cpu/ap/ap.hpp"
#include "firefly/intel64/cpu/apic/apic.hpp"
#include "firefly/intel64/int/interrupt.hpp"
#include "firefly/memory-manager/allocator.hpp"
#include "frg/manual_box.hpp"
#include "frg/spinlock.hpp"
#include "frg/vector.hpp"

namespace firefly::kernel {
namespace {
frg::manual_box<CpuData> bootCpuData;
frg::vector<CpuData *, Allocator> allCpuContexts;
frg::ticket_spinlock allCpuContextsLock;
}  // namespace

void setupCpuContext(AssemblyCpuData *context) {
    context->selfPointer = context;
    wrmsr(MSR::GsBase, reinterpret_cast<uint64_t>(context));
}

void setupBootCpuContext() {
    bootCpuData.initialize();
    setupCpuContext(bootCpuData.get());
}

void initializeBootProccessor(uint64_t stack) {
    setupBootCpuContext();
    initializeThisCpu(stack);
}

void initializeApplicationProcessor(uint64_t stack) {
    // TODO: Check with @vo1d how to do this
    auto cpuData = new (Allocator().allocate(sizeof(CpuData))) CpuData;
    setupCpuContext(cpuData);
    initializeThisCpu(stack);
}

CpuData *getCpuData(size_t k) {
    return allCpuContexts[k];
}

CpuData *getCpuData() {
    AssemblyCpuData *cpu_data;
    asm volatile("mov %%gs:0, %0"
                 : "=r"(cpu_data));
    return reinterpret_cast<CpuData *>(cpu_data);
}

void initializeThisCpu(uint64_t stack) {
    auto cpuData = getCpuData();

    allCpuContextsLock.lock();
    cpuData->cpuIndex = allCpuContexts.size();
    allCpuContextsLock.unlock();

    allCpuContexts.push(cpuData);

    logLine << "initializing CPU " << fmt::dec << cpuData->cpuIndex << '\n'
            << fmt::endl;

    core::gdt::init(cpuData->gdt);
    core::tss::init(stack);

    firefly::kernel::core::interrupt::init();

    apic::Apic::init();
}

}  // namespace firefly::kernel
