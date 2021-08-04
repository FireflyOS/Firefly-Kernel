#pragma once
#include "drivers/vga.hpp"

namespace firefly::kernel {
void start_load(const char* _str);
void end_load(const char* _str);
}  // namespace firefly::kernel