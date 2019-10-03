#include <drivers/vga.hpp>

constexpr short MAJOR_VERSION = 0;
constexpr short MINOR_VERSION = 0;

extern "C" [[noreturn]] void kernel_main() {
    Display display_driver{};
    display_driver.clear();
    display_driver.write("FireflyOS\nVersion: ");
    display_driver.write(MAJOR_VERSION + '0');
    display_driver.write('.');
    display_driver.write(MINOR_VERSION + '0');

    while (true)
        ;
}