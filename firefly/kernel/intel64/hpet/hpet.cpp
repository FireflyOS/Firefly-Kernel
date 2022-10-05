#include "firefly/intel64/hpet/hpet.hpp"

#include "firefly/intel64/acpi/acpi.hpp"
#include "firefly/intel64/int/interrupt.hpp"
#include "frg/manual_box.hpp"

namespace firefly::kernel::timer {
using core::acpi::Acpi;

namespace {
void hpet_irq_handler() {
}
frg::manual_box<HPET> hpetSingleton;
}  // namespace


HPET& HPET::accessor() {
    return *hpetSingleton;
}

// Register IRQ handler and setup HPET
void HPET::init() {
    auto const& hpetAcpiTable = reinterpret_cast<AcpiHpet*>(Acpi::accessor().mustFind("HPET"));
    hpetSingleton.initialize();
    core::interrupt::registerIRQHandler(hpet_irq_handler, 0);
}

void HPET::deinit() {
    core::interrupt::unregisterIRQHandler(0);
}

}  // namespace firefly::kernel::timer
