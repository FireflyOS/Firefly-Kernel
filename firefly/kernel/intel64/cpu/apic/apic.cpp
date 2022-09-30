#include "firefly/intel64/cpu/apic/apic.hpp"

#include <cstring>

#include "firefly/intel64/acpi/acpi.hpp"
#include "firefly/intel64/cpu/cpu.hpp"
#include "firefly/limine.hpp"
#include "firefly/memory-manager/primary/buddy.hpp"
#include "firefly/memory-manager/primary/primary_phys.hpp"
#include "libk++/bits.h"

namespace firefly::kernel::apic {
using core::acpi::Acpi;

// Write to the APIC
void Apic::write(uint32_t offset, uint32_t value) {
    auto reg = reinterpret_cast<size_t*>(reinterpret_cast<size_t>(address) + offset);
    *reinterpret_cast<volatile uint32_t*>(reg) = value;
}

// Read from the APIC
uint32_t Apic::read(uint32_t offset) const {
    auto reg = reinterpret_cast<size_t*>(reinterpret_cast<size_t>(address) + offset);
    return *reinterpret_cast<volatile uint32_t*>(reg);
}

// Set APIC register 280h to 0
// Clears the errors
void Apic::clearErrors() {
    write(0x280, 0);
}

// Set destination processor for IPI
void Apic::setIPIDest(uint32_t ap) {
    write(APIC_ICR1, (read(APIC_ICR1) & 0x00ffffff) | (ap << 24));
}

// Send APIC EOI
void Apic::sendEOI() {
    write(APIC_EOI, 0);
}

// Enable the APIC
void Apic::enable() {
    write(0xF0, read(0xF0) | BIT(8) | 0xFF);
}

void init() {
    auto const& madt = reinterpret_cast<AcpiMadt*>(Acpi::accessor().mustFind("APIC"));
    auto lapic = Apic(madt->localApicAddress);
    lapic.enable();

    // mask all interrupts
    lapic.write(APIC_LVTT, APIC_MASKED);
    lapic.write(APIC_LVTTHMR, APIC_MASKED);
    lapic.write(APIC_LVTPC, APIC_MASKED);
    lapic.write(APIC_LVT1, APIC_MASKED);
    lapic.write(APIC_LVT0, APIC_MASKED);
    lapic.write(APIC_LVTERR, APIC_MASKED);

    lapic.write(APIC_TASK_PRIORITY, 0); // Accept all INTs & exceptions

    lapic.sendEOI();
}
}  // namespace firefly::kernel::apic
