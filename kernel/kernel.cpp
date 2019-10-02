#include <drivers/vga.hpp>

<<<<<<< HEAD
extern "C" [[noreturn]] auto kernel_main() -> void {
    VgaBuffer buff{};
    buff.write('h');
    buff.write('e');
    buff.write('l');
    buff.write('l');
    buff.write('o');
=======
extern "C" [[noreturn]] void kernel_main() {
>>>>>>> 87d3ff961642c80d8b875d52b96d6a093fe1b31c
    while (true);
}