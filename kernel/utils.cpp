#include "utils.hpp"

#include <stl/utility.h>

#include <kernel.hpp>

namespace firefly::kernel {
using namespace firefly::drivers::vga;
void start_load(const char* _str) {
    cursor& crs = kernel::main::get_cursor();
    crs << "[";
    crs.set_foreground_color(color::blue);
    crs << "START";
    crs.set_foreground_color(color::white);
    crs << "]" << _str << "\n";
}

void end_load(const char* _str) {
    cursor& crs = kernel::main::get_cursor();
    // cursor& crs = get_cursor();
    crs << "[";
    crs.set_foreground_color(color::green);
    crs << "OK";
    crs.set_foreground_color(color::white);
    crs << "]" << _str << "\n";
}
}  // namespace firefly::kernel