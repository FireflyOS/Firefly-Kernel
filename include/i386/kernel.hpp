#pragma once

#include <i386/drivers/vga.hpp>

namespace firefly::kernel::main {
firefly::drivers::vga::cursor &internal_cursor_handle();
}