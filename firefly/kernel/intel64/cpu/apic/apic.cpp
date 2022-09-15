#include "firefly/intel64/acpi/acpi.hpp"

namespace firefly::kernel::apic {
using core::acpi::Acpi;

void init() {
    auto const& madt = reinterpret_cast<AcpiMadt*>(Acpi::accessor().mustFind("APIC"));
    auto const& results = madt->enumerate();
    auto apics = results.get<0>();
    auto io_apics = results.get<1>();

    // This is an array, the size is always the hardcoded 4.
    // This'll just transfer over to the vector and provide accurate results.
    // Until then, don't treat this like there are 'apics.size()' apics
    if (!apics.empty())
        info_logger << info_logger.format("Found %d local apics\n", apics.size());

    if (!io_apics.empty())
        info_logger << info_logger.format("Found %d io apics\n", io_apics.size());
}
}  // namespace firefly::kernel::apic