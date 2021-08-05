#pragma once

#include <drivers/vga.hpp>

namespace firefly::libkern {
using namespace firefly::drivers::vga;

/* This allows the kernel to get a global reference to the cursor object */
static firefly::drivers::vga::cursor cout;
const char endl = '\n';

inline firefly::drivers::vga::cursor& get_cursor_handle() {
    return cout;
}

inline void globalize_vga_writer() {
    drivers::vga::cursor crs = { color::white, color::black, 0, 0 };
    cout = crs;
}

template <typename... Tys>
void print(Tys... args) {
    ((cout << args << ' '), ...);
    cout << endl;
}

}  // namespace firefly::libkern