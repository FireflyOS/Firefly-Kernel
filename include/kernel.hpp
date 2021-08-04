#pragma once

#include <drivers/vga.hpp>

namespace firefly::kernel::main {
firefly::drivers::vga::cursor &get_cursor();
}