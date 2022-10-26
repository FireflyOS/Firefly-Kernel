#include "firefly/intel64/hpet/hpet.hpp"

#include "firefly/intel64/acpi/acpi.hpp"
#include "firefly/intel64/cpu/apic/apic.hpp"
#include "firefly/intel64/int/interrupt.hpp"
#include "firefly/logger.hpp"
#include "frg/manual_box.hpp"

namespace firefly::kernel::timer {
using core::acpi::Acpi;

namespace {
void hpet_irq2_handler() {
    logLine << "IRQ #2\n"
            << fmt::endl;
}
void hpet_irq8_handler() {
    logLine << "IRQ #8\n"
            << fmt::endl;
}

frg::manual_box<HPET> hpetSingleton;
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

// Register IRQ handler and setup HPET
void HPET::init() {
    if (hpetSingleton.valid()) panic("Tried to initialize HPET twice");
    auto const& hpetAcpiTable = reinterpret_cast<AcpiHpet*>(Acpi::accessor().mustFind("HPET"));
    hpetSingleton.initialize(hpetAcpiTable->address, reinterpret_cast<uint16_t>(hpetAcpiTable->minimumClockTicks));
    auto hpet = hpetSingleton.get();
    apic::enableIRQ(0);
    core::interrupt::registerIRQHandler(hpet_irq2_handler, apic::IOApic::getGSI(0));

    hpet->initialize();
    hpet->enable();
    hpet->write(timer_config(0), (apic::IOApic::getGSI(0) << 9) | BIT(2) | BIT(3) | BIT(6));
    hpet->write(timer_comparator(0), hpet->read(MAIN_COUNTER_VALUE_REG) + hpet->period);
    hpet->write(timer_comparator(0), hpet->period / 2);
}

void HPET::deinit() {
    core::interrupt::unregisterIRQHandler(apic::IOApic::getGSI(0));
}

}  // namespace firefly::kernel::timer
