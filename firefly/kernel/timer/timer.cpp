#include "firefly/timer/timer.hpp"

#include "firefly/intel64/cpu/apic/apic.hpp"
#include "firefly/intel64/hpet/hpet.hpp"
#include "firefly/intel64/int/interrupt.hpp"
#include "firefly/intel64/pit/pit.hpp"
#include "firefly/logger.hpp"
#include "firefly/panic.hpp"
#include "firefly/scheduler/scheduler.hpp"

namespace firefly::kernel {
// TODO: rework this, probably a better solution,
// also not use HPET in sleep maybe?
namespace timer {

namespace {
// This will just get increased
static volatile uint64_t ticks = 0;
static uint32_t timer_quantum;
}  // namespace

void timer_irq(uint8_t int_num, RegisterContext* regs) {
    // debugLine << "LApic Timer IRQ\n"
    // << fmt::endl;
    apic::ApicTimer::accessor().oneShotTimer(timer_quantum);
    scheduler::Scheduler::accessor().schedule(regs);
}

void init() {
    resetTicks();
    HPET::init();
    apic::ApicTimer::init();
    core::interrupt::registerIRQHandler(timer_irq, 0);
    timer_quantum = apic::ApicTimer::accessor().calibrate(20000);
    // pit::init();
    return;
    // panic("No usable timer found");
}

void startTimer() {
    apic::ApicTimer::accessor().oneShotTimer(timer_quantum);
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
