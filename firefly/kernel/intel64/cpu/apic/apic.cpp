#include "firefly/intel64/cpu/apic/apic.hpp"

#include "firefly/intel64/acpi/acpi.hpp"
#include "firefly/limine.hpp"
#include "libk++/bits.h"

namespace firefly::kernel::apic {
using core::acpi::Acpi;

struct limine_smp_request smp_request {
    .id = LIMINE_SMP_REQUEST, .revision = 0, .response = nullptr
};

void Apic::write(uint32_t offset, uint32_t value) {
    auto reg = reinterpret_cast<size_t*>((size_t)address + offset);
    *reinterpret_cast<uint32_t*>(reg) = value;
}

uint32_t Apic::read(uint32_t offset) const {
    auto reg = reinterpret_cast<size_t*>((size_t)address + offset);
    return *reinterpret_cast<uint32_t*>(reg);
}

void Apic::enable() {
	write(0xF0, read(0xF0) | BIT(8));
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

    for (std::size_t i = 0; i < apics.size(); i++) {
        auto const entry = apics[i];
        if (entry->apicId == bspId) continue;
        SerialLogger::log() << "non bsp apic found, ID = " << entry->apicId << "\n";
    }
}
}  // namespace firefly::kernel::apic
