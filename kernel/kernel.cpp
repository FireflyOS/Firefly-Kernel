#include "drivers/vga.hpp"

extern "C" [[noreturn]] auto kernel_main() -> void {
    VgaBuffer buff{};
    buff.write('h');
    buff.write('e');
    buff.write('l');
    buff.write('l');
    buff.write('o');
    while (true);
}