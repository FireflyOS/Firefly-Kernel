#include <drivers/vga.hpp>
#include "array.h"

constexpr short MAJOR_VERSION = 0;
constexpr short MINOR_VERSION = 0;

void write_ff_info(Display& display_driver) {
    display_driver.clear();
    display_driver << "FireflyOS\nVersion: " << '0' + MAJOR_VERSION
                   << "." << MINOR_VERSION + '0' << "\nContributors:";

    firefly::std::array<const char*, 7> arr = { 
        "Lime      ", "JohnkaS", "EyeDevelop", "4lpha", "Burokkoru ", "extation", "RedEye2D" };

    for (size_t i = 0; i < arr.max_size(); i++) {
        if (i % 2 == 0) {
            display_driver << "\n\t";
        }
        display_driver << arr[i] << "  ";
    }
}

extern "C" [[noreturn]] void kernel_main() {
    Display display_driver{};
    write_ff_info(display_driver);

    while (true)
        ;
}