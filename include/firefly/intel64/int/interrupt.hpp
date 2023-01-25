#pragma once

#include <cstdint>

#include "firefly/intel64/cpu/cpu.hpp"

namespace firefly::kernel::core::interrupt {

void init();

// register IRQ handler
// No need to call update() as we just write it to a table with handlers
// for the actual interrupt handlers
void registerIRQHandler(void (*handler)(ContextRegisters*), uint8_t irq);
void unregisterIRQHandler(uint8_t irq);

namespace change {
extern "C" void update(void (*handler)(), uint16_t cs, uint8_t type, uint8_t index);
}
}  // namespace firefly::kernel::core::interrupt
