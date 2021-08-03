#pragma once
#include "drivers/vga.hpp"

namespace firefly::kernel
{
    void start_load(drivers::vga::cursor& crs, const char* _str);
    void end_load(drivers::vga::cursor& crs, const char* _str);
}  // namespace firefly::kernel