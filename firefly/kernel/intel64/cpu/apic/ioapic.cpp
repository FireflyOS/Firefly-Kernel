#include <cstring>

#include "cstdlib/cassert.h"
#include "firefly/drivers/ports.hpp"
#include "firefly/intel64/acpi/acpi.hpp"
#include "firefly/intel64/acpi/tables/madt.hpp"
#include "firefly/intel64/cpu/apic/apic.hpp"
#include "firefly/intel64/cpu/cpu.hpp"
#include "firefly/limine.hpp"
#include "firefly/logger.hpp"
#include "firefly/memory-manager/allocator.hpp"
#include "firefly/memory-manager/primary/buddy.hpp"
#include "firefly/memory-manager/primary/primary_phys.hpp"
#include "frg/array.hpp"
#include "frg/manual_box.hpp"
#include "libk++/bits.h"

namespace firefly::kernel::apic {
using core::acpi::Acpi;

namespace {
AcpiMadt* madt = nullptr;
frg::manual_box<frg::vector<IOApic, Allocator>> ioapicsSingleton;
}  // namespace


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
    assert_truth(madt != nullptr);
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
void IOApic::updateIRQ(uint8_t irq) {
    RedirectionEntry redEnt = {};
    redEnt.vector = LVT_BASE + irq;
    // redEnt.destination = cpu id here;
    redEnt.delvMode = IOAPIC_DELMODE_FIXED;
    redEnt.destMode = DestinationMode::Physical;

    write((IOAPIC_REDTBL_BASE + irq * 2), redEnt.lowerDword);
    write((IOAPIC_REDTBL_BASE + irq * 2 + 1), redEnt.upperDword);
}

void enableIRQ(uint8_t irq) {
    uint64_t gsi = IOApic::getGSI(irq);
    logLine << "IRQ #" << fmt::dec << irq << " ==> GSI #" << fmt::dec << gsi << "\n"
            << fmt::endl;

    for (size_t i = 0; i < ioapicsSingleton->size(); i++) {
        auto ioapic = ioapicsSingleton->data()[i];
        if (gsi < ioapic.getGlobalInterruptBase()) {
            ioapic.updateIRQ(gsi);
            return;
        }
    }

    panic("No IOApic found for IRQ");
    return;
}

void IOApic::initAll() {
    using core::acpi::Acpi;
    madt = reinterpret_cast<AcpiMadt*>(Acpi::accessor().mustFind("APIC"));
    ioapicsSingleton.initialize();

    auto const result = madt->enumerate();
    auto const ioapics = result.get<1>();
    for (size_t i = 0; i < 1; i++) {
        auto entry = ioapics[i];
        IOApic ioapic = IOApic(entry->ioApicAddress, entry->ioApicId, entry->globalInterruptBase);
        ioapicsSingleton->push(ioapic);
    }
}


}  // namespace firefly::kernel::apic
