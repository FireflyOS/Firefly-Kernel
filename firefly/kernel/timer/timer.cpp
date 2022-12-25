#include "firefly/timer/timer.hpp"

#include "firefly/intel64/cpu/apic/apic.hpp"
#include "firefly/intel64/hpet/hpet.hpp"
#include "firefly/intel64/int/interrupt.hpp"
#include "firefly/intel64/pit/pit.hpp"
#include "firefly/logger.hpp"
#include "firefly/panic.hpp"

namespace firefly::kernel {
// TODO: rework this, probably a better solution
namespace timer {

namespace {
// This will just get increased
static volatile uint64_t ticks = 0;
}  // namespace

void timer_irq() {
    debugLine << "timer\n"
              << fmt::endl;
}

void init() {
    resetTicks();
    HPET::init();
    core::interrupt::registerIRQHandler(timer_irq, 0);
    if (apic::ApicTimer::isAvailable()) {
        apic::ApicTimer::init();
    } else {
        pit::init();
    }
    return;
    // panic("No usable timer found");
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
