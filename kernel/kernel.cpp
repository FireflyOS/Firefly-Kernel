#include <idt.hpp>
#include "drivers/keyboard.hpp"
#include "drivers/vga.hpp"
#include "stl/array.h"

[[maybe_unused]] constexpr short MAJOR_VERSION = 0;
[[maybe_unused]] constexpr short MINOR_VERSION = 0;
constexpr const char* VERSION_STRING = "0.0";

void write_ff_info(Display& display_driver) {
    display_driver.clear();
    display_driver << "FireflyOS\nVersion: " << VERSION_STRING << "\nContributors:";

    firefly::std::array<const char*, 7> arr = {
        "Lime\t  ", "JohnkaS", "EyeDevelop", "4lpha", "Burokkoru ", "extation", "RedEye2D"
    };

    for (size_t i = 0; i < arr.max_size(); i++) {
        if (i % 2 == 0) {
            display_driver << "\n\t";
        }
        display_driver << arr[i] << "  ";
    }
    display_driver << "\n";
}

extern "C" [[noreturn]] void kernel_main() {
    Display display_driver{};
    // write_ff_info(display_driver);
    Keyboard keyboard_driver{ display_driver };

    init_idt();

    while (true) {
        auto key = keyboard_driver.get_scancode();
        if (!key.has_value()) {
            continue;
        }
        keyboard_driver.handle_input(*key, display_driver);
    }
}