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

void IOApic::init() {
    SerialLogger::log() << "Initialising IOAPIC" << logger::endl;
    SerialLogger::log() << "\tID: " << SerialLogger::log().hex(this->ID) << logger::endl;
    SerialLogger::log() << "\tVER: " << SerialLogger::log().hex(this->ioApicVersion) << logger::endl;
}

void IOApic::enableIRQ(uint8_t irq) {
    RedirectionEntry redEnt = {};

    redEnt.vector = LVT_BASE + irq;
    // redEnt.destination = cpu id here;
    redEnt.delvMode = IOAPIC_DELMODE_FIXED;
    redEnt.destMode = DestinationMode::Physical;

    write((IOAPIC_REDTBL_BASE + irq * 2), redEnt.lowerDword);
    write((IOAPIC_REDTBL_BASE + irq * 2 + 1), redEnt.upperDword);
}

void IOApic::initAll() {
    using core::acpi::Acpi;
    auto const& madt = reinterpret_cast<AcpiMadt*>(Acpi::accessor().mustFind("APIC"));
    auto const ioapics = madt->enumerate().get<1>();
    // for (uint32_t i = 0; i < ioapics.size(); i++) {
    // auto const entry = ioapics[i];
    auto const entry = ioapics[0];
    auto ioapic = IOApic(entry->ioApicAddress, entry->ioApicId, entry->globalInterruptBase);
    ioapic.init();
    ioapic.enableIRQ(0);
    ioapic.enableIRQ(1);
    ioapic.enableIRQ(2);
    ioapic.enableIRQ(3);
    // }
}


}  // namespace firefly::kernel::apic
