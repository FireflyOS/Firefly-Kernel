#pragma once

#include <x86_64/drivers/vga.hpp>

namespace firefly::kernel::main {
firefly::drivers::vga::cursor &internal_cursor_handle();
}