#include "firefly/timer/timer.hpp"

#include "firefly/intel64/hpet/hpet.hpp"
#include "firefly/intel64/pit/pit.hpp"
#include "firefly/logger.hpp"
#include "firefly/panic.hpp"

namespace firefly::kernel {
// TODO: rework this, probably a better solution
namespace timer {

namespace {
// This will just get increased
static volatile uint64_t ticks = 0;
// Frequency in Hz
static uint64_t frequency = 0;
}  // namespace
void init() {
    resetTicks();
    HPET::init();
    // pit::init();
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

void setFrequency(uint64_t _frequency) {
    frequency = _frequency;
}

void sleep(uint64_t ms) {
    usleep(ms * 1000);
}

void usleep(uint64_t us) {
    uint64_t aTicks = 0;
    // min. == 10 us
    if (us < 10) us = 10;
    if (frequency < 1000 * 100) {
        aTicks = ticks + (frequency / 1000) * (us / 1000);
    } else {
        aTicks = ticks + (frequency / 1000 * 100) * (us / 10);
    }
    while (ticks != aTicks) {
        asm volatile("hlt");
    }
}
}  // namespace timer
}  // namespace firefly::kernel
