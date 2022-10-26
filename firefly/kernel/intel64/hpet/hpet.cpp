#include "firefly/intel64/hpet/hpet.hpp"

#include "firefly/intel64/acpi/acpi.hpp"
#include "firefly/intel64/cpu/apic/apic.hpp"
#include "firefly/intel64/int/interrupt.hpp"
#include "frg/manual_box.hpp"

namespace firefly::kernel::timer {
using core::acpi::Acpi;

namespace {
void hpet_irq2_handler() {
}
void hpet_irq8_handler() {
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

// Register IRQ handler and setup HPET
void HPET::init() {
    auto const& hpetAcpiTable = reinterpret_cast<AcpiHpet*>(Acpi::accessor().mustFind("HPET"));
    hpetSingleton.initialize(hpetAcpiTable->address);
    auto hpet = hpetSingleton.get();

    core::interrupt::registerIRQHandler(hpet_irq2_handler, apic::IOApic::getGSI(0));
    core::interrupt::registerIRQHandler(hpet_irq8_handler, apic::IOApic::getGSI(8));
}

void HPET::deinit() {
    core::interrupt::unregisterIRQHandler(apic::IOApic::getGSI(0));
    core::interrupt::unregisterIRQHandler(apic::IOApic::getGSI(8));
}

}  // namespace firefly::kernel::timer
