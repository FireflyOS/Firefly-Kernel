#include "firefly/timer/hpet.hpp"

#include "firefly/intel64/acpi/acpi.hpp"
#include "firefly/intel64/int/interrupt.hpp"

namespace {
void hpet_irq_handler() {
}
}  // namespace

namespace firefly::kernel::timer {

using core::acpi::Acpi;

// Register IRQ handler and setup HPET
void HPET::init() {
    auto const& hpetAcpiTable = reinterpret_cast<AcpiHpet*>(Acpi::accessor().mustFind("HPET"));

    core::interrupt::registerIRQHandler(hpet_irq_handler, 0);
}

void HPET::deinit() {
    core::interrupt::unregisterIRQHandler(0);
}

}  // namespace firefly::kernel::timer
