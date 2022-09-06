#include "firefly/intel64/acpi/acpi.hpp"

namespace firefly::kernel::apic {
using core::acpi::Acpi;

void init() {
	auto const madt = reinterpret_cast<AcpiMadt*>(Acpi::accessor().mustFind("APIC"));
}
}  // namespace firefly::kernel::apic