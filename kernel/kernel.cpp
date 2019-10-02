#include <drivers/vga.hpp>

extern "C" [[noreturn]] void kernel_main() {
    Display x{};
    x.clear();
    x.set_background_color(color::green);
    x.set_foreground_color(color::red);

    for (int i = 0; i < 1200; i++) {
        x.write('@');
    }

    // x.write("hello world!");
    // x.clear();
    // (void)x;
    // for (int i = 0; i < 50; i++) {
    //     // x.write(' ');
    // }
    // x.clear();
    // *reinterpret_cast<char *>(0xb8000) = 'x';
    // *reinterpret_cast<char*>(0xb8001) = 0x20;

    while (true)
        ;
}