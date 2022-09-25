#include "firefly/intel64/cpu/apic/apic.hpp"

#include <cstring>

#include "firefly/intel64/acpi/acpi.hpp"
#include "firefly/limine.hpp"
#include "firefly/memory-manager/primary/buddy.hpp"
#include "firefly/memory-manager/primary/primary_phys.hpp"
#include "libk++/bits.h"

// defined in trampoline.asm
extern char smp_trampoline_start[];
extern size_t smp_trampoline_size;

namespace firefly::kernel::apic {
using core::acpi::Acpi;

// Contains the BSP ID
struct limine_smp_request smp_request {
    .id = LIMINE_SMP_REQUEST, .revision = 0, .response = nullptr
};

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
    auto const bspId = smp_request.response->bsp_lapic_id;
    auto lapic = Apic(madt->localApicAddress);
    lapic.enable();

    // This is an array, the size is always the hardcoded 4.
    // This'll just transfer over to the vector and provide accurate results.
    // Until then, don't treat this like there are 'apics.size()' apics
    if (!apics.empty())
        ConsoleLogger::log() << ConsoleLogger::log().format("Found %d local apics\n", apics.size());

    if (!io_apics.empty())
        ConsoleLogger::log() << ConsoleLogger::log().format("Found %d io apics\n", io_apics.size());

    void* trampoline = nullptr;
    if (trampoline == nullptr) {
        trampoline = mm::Physical::allocate(smp_trampoline_size);
        std::memcpy(trampoline, smp_trampoline_start, smp_trampoline_size);
    }

    for (std::size_t i = 0; i < apics.size(); i++) {
        auto const entry = apics[i];
        // skip apic of the BSP as we're already running on it
        if (entry->apicId == bspId) continue;
        // send init IPI
        lapic.clearErrors();
        lapic.setIPIDest(i);
        lapic.write(LAPIC_REG_ICR0, 0x4500);

        // wait for delivery of IPI
        while (lapic.read(LAPIC_REG_ICR0) & BIT(12)) {
            asm volatile("pause");
        }

        lapic.setIPIDest(i);
        lapic.write(LAPIC_REG_ICR0, ((((size_t)trampoline) - AddressLayout::Code ) / 4096) | 0x4600);

        // wait for delivery of IPI
        while (lapic.read(LAPIC_REG_ICR0) & BIT(12)) {
            asm volatile("pause");
        }
    }
}
}  // namespace firefly::kernel::apic
