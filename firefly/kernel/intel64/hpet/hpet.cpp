#include "firefly/intel64/hpet/hpet.hpp"

#include "firefly/intel64/acpi/acpi.hpp"
#include "firefly/intel64/cpu/apic/apic.hpp"
#include "firefly/intel64/cpu/cpu.hpp"
#include "firefly/intel64/int/interrupt.hpp"
#include "firefly/logger.hpp"
#include "firefly/memory-manager/allocator.hpp"
#include "frg/manual_box.hpp"

namespace firefly::kernel::timer {
using core::acpi::Acpi;

namespace {
uint64_t ms_slept = 0;

void hpet_irq2_handler() {
    ms_slept++;
    // if (ms_slept % 100 == 0) debugLine << fmt::hex << ms_slept << '\n' << fmt::endl;
}

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
    period = read(GENERAL_CAPS_REG) & 0xFFFFFFFF;
    frequency = 10 ^ 15 / period;
}

// Set bit 0
void HPET::enable() {
    write(GENERAL_CONF_REG, read(GENERAL_CONF_REG) | BIT(0));
}

// Clear bit 0
void HPET::disable() {
    write(GENERAL_CONF_REG, read(GENERAL_CONF_REG) & ~BIT(0));
}

void HPET::sleep_ms(uint32_t ms) {
    ms_slept = 0;

    enable();
    while (ms_slept != ms) {
        asm volatile("pause"
                     :
                     :
                     : "memory");
    }
    disable();
}

void HPETTimer::setPeriodicMode() {
    if (!periodicSupport) panic("Timer doesn't have support for periodic interrupts");
    auto cfg = HPET::accessor().read(timer_config(timerNum));
    HPET::accessor().write(timer_config(timerNum), cfg);
}

void HPETTimer::setOneshotMode() {
    auto cfg = HPET::accessor().read(timer_config(timerNum));
    HPET::accessor().write(timer_config(timerNum), cfg);
}

// Register IRQ handler and setup HPET
void HPET::init() {
    if (hpetSingleton.valid()) panic("Tried to initialize HPET twice");
    auto const& hpetAcpiTable = reinterpret_cast<AcpiHpet*>(Acpi::accessor().mustFind("HPET"));
    hpetSingleton.initialize(hpetAcpiTable->address, reinterpret_cast<uint16_t>(hpetAcpiTable->minimumClockTicks));
    hpetTimers.initialize();

    auto hpet = hpetSingleton.get();

    size_t timersSize = 3;  // TODO: bits 12-8 in general caps
    for (size_t i = 0; i < timersSize; i++) {
        bool periodic = (hpet->read(timer_config(i)) >> 4) & 1UL;
        hpetTimers->push(HPETTimer(periodic, i));
    }

    apic::enableIRQ(0);
    core::interrupt::registerIRQHandler(hpet_irq2_handler, apic::IOApic::getGSI(0));

    hpet->initialize();
    hpet->disable();
    debugLine << "timer start\n"
              << fmt::endl;
    hpet->sleep_ms(1000);
    debugLine << "timer stop\n"
              << fmt::endl;
}

void HPET::deinit() {
    core::interrupt::unregisterIRQHandler(apic::IOApic::getGSI(0));
}

}  // namespace firefly::kernel::timer
