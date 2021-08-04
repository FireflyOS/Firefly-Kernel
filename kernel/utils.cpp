#include "utils.hpp"

#include <stl/utility.h>

namespace firefly::kernel {
    using namespace firefly::drivers::vga;
    void start_load(cursor& crs, const char* _str) {
        crs << "[";
        crs.set_foreground_color(color::blue);
        crs << "START";
        crs.set_foreground_color(color::white);
        crs << "]" << _str << "\n";
    }

    void end_load(cursor& crs, const char* _str) {
        crs << "[";
        crs.set_foreground_color(color::green);
        crs << "OK";
        crs.set_foreground_color(color::white);
        crs << "]" << _str << "\n";
    }
}  // namespace firefly::kernel