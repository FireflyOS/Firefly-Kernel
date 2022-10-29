#pragma once

#include <cstdint>

namespace firefly::kernel::timer {
// This MUST setup a timer, else panics
void init();

void tick();
void resetTicks();
void setFrequency(uint64_t _frequency);

void sleep(uint64_t ms);
void usleep(uint64_t us);
}  // namespace firefly::kernel::timer
