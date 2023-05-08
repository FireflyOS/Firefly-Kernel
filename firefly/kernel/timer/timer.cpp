#include "firefly/timer/timer.hpp"

#include "firefly/intel64/cpu/apic/apic.hpp"
#include "firefly/intel64/cpu/cpu.hpp"
#include "firefly/intel64/hpet/hpet.hpp"
#include "firefly/intel64/int/interrupt.hpp"
#include "firefly/intel64/pit/pit.hpp"
#include "firefly/logger.hpp"
#include "firefly/memory-manager/allocator.hpp"
#include "firefly/memory-manager/primary/primary_phys.hpp"
#include "firefly/panic.hpp"
// #include "firefly/tasks/scheduler.hpp"

namespace firefly::kernel {
namespace timer {

namespace {
// This will just get increased
static volatile uint64_t ticks = 0;
static volatile uint32_t ticks_20ms = 0;
}  // namespace

void timer_irq(ContextRegisters* stack) {
    const auto& data = reinterpret_cast<CpuData*>(rdmsr(MSR::GsBase));
    data->scheduler->onSchedule(stack, data->cpuIndex);
    start();
}

void start() {
    apic::ApicTimer::accessor().oneShotTimer(ticks_20ms);
}

void init() {
    resetTicks();
    HPET::init();
    core::interrupt::registerIRQHandler(timer_irq, 0);
    if (apic::ApicTimer::isAvailable()) {
        apic::ApicTimer::init();
        ticks_20ms = 200 * apic::ApicTimer::accessor().calibrate(100);
    } else {
        pit::init();
    }
}


// Probably call this function in the IRQ handler, or whatever the timer fires
void tick() {
    ticks = ticks + 1;
}

void resetTicks() {
    ticks = 0;
}

void msleep(uint64_t ms) {
    usleep(ms * 1000);
}

void usleep(uint64_t us) {
    HPET::accessor().usleep(us);
}


}  // namespace timer
}  // namespace firefly::kernel
