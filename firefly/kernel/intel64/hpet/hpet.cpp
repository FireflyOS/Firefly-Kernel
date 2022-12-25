#include "firefly/intel64/hpet/hpet.hpp"

#include "firefly/intel64/acpi/acpi.hpp"
#include "firefly/intel64/cpu/apic/apic.hpp"
#include "firefly/intel64/cpu/cpu.hpp"
#include "firefly/intel64/int/interrupt.hpp"
#include "firefly/logger.hpp"
#include "firefly/memory-manager/allocator.hpp"
#include "firefly/timer/timer.hpp"
#include "frg/manual_box.hpp"

namespace firefly::kernel::timer {
using core::acpi::Acpi;

namespace {
frg::manual_box<HPET> hpetSingleton;
frg::manual_box<frg::vector<HPETTimer, Allocator>> hpetTimers;
}  // namespace


HPET& HPET::accessor() {
    return *hpetSingleton;
}

void HPET::write(const uint64_t offset, const uint64_t value) {
    auto reg = reinterpret_cast<size_t*>(reinterpret_cast<size_t>(address) + offset);
    *reinterpret_cast<volatile uint64_t*>(reg) = value;
}

uint64_t HPET::read(const uint64_t offset) const {
    auto reg = reinterpret_cast<size_t*>(reinterpret_cast<size_t>(address) + offset);
    return *reinterpret_cast<volatile uint64_t*>(reg);
}

void HPET::initialize() {
    period = (read(GENERAL_CAPS_REG) >> 32);
}

// Set bit 0
void HPET::enable() {
    write(GENERAL_CONF_REG, read(GENERAL_CONF_REG) | BIT(0));
}

// Clear bit 0
void HPET::disable() {
    write(GENERAL_CONF_REG, read(GENERAL_CONF_REG) & ~BIT(0));
}

void HPETTimer::setPeriodicMode() {
    if (!periodicSupport) panic("Timer doesn't have support for periodic interrupts");
    auto cfg = HPET::accessor().read(timer_config(timerNum));
    cfg |= (1 << 3) | (1 << 6);
    HPET::accessor().write(timer_config(timerNum), cfg);
}

void HPETTimer::setOneshotMode() {
    auto cfg = HPET::accessor().read(timer_config(timerNum));
    HPET::accessor().write(timer_config(timerNum), cfg);
}

void HPETTimer::setIoApicOutput(uint8_t output) {
    auto cfg = HPET::accessor().read(timer_config(timerNum));
    cfg |= (output << 9);
    HPET::accessor().write(timer_config(timerNum), cfg);
}

void HPETTimer::enable() {
    auto cfg = HPET::accessor().read(timer_config(timerNum));
    cfg |= (1 << 2);
    HPET::accessor().write(timer_config(timerNum), cfg);
}

void HPETTimer::disable() {
    auto cfg = HPET::accessor().read(timer_config(timerNum));
    cfg |= (0 << 2);
    HPET::accessor().write(timer_config(timerNum), cfg);
}


// Setup HPET
// Includes unused things, that may be helpful later
void HPET::init() {
    if (hpetSingleton.valid())
        panic("Tried to initialize HPET twice");
    auto const& hpetAcpiTable = reinterpret_cast<AcpiHpet*>(Acpi::accessor().mustFind("HPET"));
    hpetSingleton.initialize(hpetAcpiTable->address, reinterpret_cast<uint16_t>(hpetAcpiTable->minimumClockTicks));
    hpetTimers.initialize();

    auto hpet = hpetSingleton.get();
    hpet->write(GENERAL_CONF_REG, 0);
    hpet->write(MAIN_COUNTER_VALUE_REG, 0);
    hpet->disable();

    size_t timersSize = 3;  // TODO: bits 12-8 in general caps
    for (size_t i = 0; i < timersSize; i++) {
        bool periodic = (hpet->read(timer_config(i)) >> 4) & 1UL;
        hpetTimers->push(HPETTimer(periodic, i));
    }

    auto hpetTimer = hpetTimers->data()[0];
    hpetTimer.setPeriodicMode();
    hpetTimer.enable();

    hpet->initialize();
    hpet->enable();
}

void HPET::deinit() {
    auto hpet = hpetSingleton.get();
    hpet->disable();
}

}  // namespace firefly::kernel::timer
