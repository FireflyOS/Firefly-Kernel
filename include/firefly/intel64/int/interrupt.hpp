#pragma once

namespace firefly::kernel::core::interrupt {
void init();

// test interrupt handler
void test_int();

// register IRQ handler
// No need to call update() as we just write it to a table with handlers
// for the actual interrupt handler
void registerIRQHandler(void (*handler)(), uint8_t irq);

namespace change {
extern "C" void update(void (*handler)(), uint16_t cs, uint8_t type, uint8_t index);
}
}  // namespace firefly::kernel::core::interrupt
