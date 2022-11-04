#pragma once

#include <cstdint>

#include "firefly/scheduler/scheduler.hpp"

namespace firefly::kernel::core::interrupt {
constexpr uint8_t IPI_SCHEDULE = 0xFD;

void init();

// register IRQ handler
// No need to call update() as we just write it to a table with handlers
// for the actual interrupt handler
void registerIRQHandler(void (*handler)(uint8_t int_num, RegisterContext* regs), uint8_t irq);
void unregisterIRQHandler(uint8_t irq);

// register IPI handler
void registerInterruptHandler(void (*handler)(uint8_t int_num, RegisterContext* regs), uint8_t int_num);
void unregisterInterruptHandler(uint8_t int_num);

namespace change {
extern "C" void update(void (*handler)(uint8_t int_num, RegisterContext* regs), uint16_t cs, uint8_t type, uint8_t index);
}
}  // namespace firefly::kernel::core::interrupt
