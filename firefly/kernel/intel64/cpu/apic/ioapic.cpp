#include <cstring>

#include "firefly/drivers/ports.hpp"
#include "firefly/intel64/acpi/acpi.hpp"
#include "firefly/intel64/cpu/apic/apic.hpp"
#include "firefly/intel64/cpu/cpu.hpp"
#include "firefly/limine.hpp"
#include "firefly/logger.hpp"
#include "firefly/memory-manager/primary/buddy.hpp"
#include "firefly/memory-manager/primary/primary_phys.hpp"
#include "frg/array.hpp"
#include "libk++/bits.h"

namespace firefly::kernel::apic {
using core::acpi::Acpi;
// Write to the IOAPIC
void IOApic::write(uint8_t offset, uint32_t value) {
    *reinterpret_cast<volatile uint32_t*>(address) = offset;
    *reinterpret_cast<volatile uint32_t*>(address + 0x10) = value;
}

// Read from the IOAPIC
uint32_t IOApic::read(uint8_t offset) const {
    *reinterpret_cast<volatile uint32_t*>(address) = offset;
    return *reinterpret_cast<volatile uint32_t*>(address + 0x10);
}

// Check for MADT source override
// or return one-to-one GSI
uint8_t IOApic::getGSI(uint8_t irq) {
    auto const result = madt->enumerate();
    auto const sourceOverrides = result.get<2>();
    for (size_t i = 0; i < 6; i++) {
        auto entry = sourceOverrides[i];
        if (entry != nullptr && entry->source == irq) {
            return entry->gsi;
        }
    }
    return irq;
}

// map IRQ to BSP (for now)
void IOApic::enableIRQ(uint8_t irq) {
    RedirectionEntry redEnt = {};
    uint64_t gsi = getGSI(irq);
    logLine << "IRQ #" << irq << " ==> GSI #" << gsi << "\n"
            << fmt::endl;

    redEnt.vector = LVT_BASE + gsi;
    // redEnt.destination = cpu id here;
    redEnt.delvMode = IOAPIC_DELMODE_FIXED;
    redEnt.destMode = DestinationMode::Physical;

    write((IOAPIC_REDTBL_BASE + gsi * 2), redEnt.lowerDword);
    write((IOAPIC_REDTBL_BASE + gsi * 2 + 1), redEnt.upperDword);
}

void IOApic::initAll() {
    using core::acpi::Acpi;
    auto const& madt = reinterpret_cast<AcpiMadt*>(Acpi::accessor().mustFind("APIC"));
    auto const result = madt->enumerate();
    auto const ioapics = result.get<1>();
    for (size_t i = 0; i < 1; i++) {
        auto entry = ioapics[i];
        IOApic ioapic = IOApic(entry->ioApicAddress, entry->ioApicId, entry->globalInterruptBase, madt);
        logLine << "Timer GSI: #" << (uint64_t)ioapic.getGSI(0) << "\n"
                << fmt::endl;
        ioapic.enableIRQ(1);
    }
}


}  // namespace firefly::kernel::apic