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
    write(LAPIC_REG_ICR1, (read(LAPIC_REG_ICR1) & 0x00ffffff) | (ap << 24));
}

// Send APIC EOI
void Apic::sendEOI() {
    write(LAPIC_REG_EOI, 0);
}

// Enable the APIC
void Apic::enable() {
    write(0xF0, read(0xF0) | BIT(8) | 0xFF);
}

void init() {
    auto const& madt = reinterpret_cast<AcpiMadt*>(Acpi::accessor().mustFind("APIC"));
    auto const& results = madt->enumerate();
    auto apics = results.get<0>();
    auto io_apics = results.get<1>();
    auto lapic = Apic(madt->localApicAddress);
    lapic.enable();

    // This is an array, the size is always the hardcoded 4.
    // This'll just transfer over to the vector and provide accurate results.
    // Until then, don't treat this like there are 'apics.size()' apics
    // if (!apics.empty())
        // ConsoleLogger::log() << ConsoleLogger::log().format("Found %d local apics\n", apics.size());

    // if (!io_apics.empty())
        // ConsoleLogger::log() << ConsoleLogger::log().format("Found %d io apics\n", io_apics.size());
}
}  // namespace firefly::kernel::apic
