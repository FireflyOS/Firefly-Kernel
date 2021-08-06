#include <i386/libk++/ios.h>
#include <i386/libk++/iostream.h>
#include <stdint.h>
#include <stl/array.h>

#include <i386/drivers/vga.hpp>
#include <i386/init/init.hpp>
#include <i386/multiboot2.hpp>

[[maybe_unused]] constexpr short MAJOR_VERSION = 0;
[[maybe_unused]] constexpr short MINOR_VERSION = 0;
constexpr const char *VERSION_STRING = "0.0";

namespace firefly::kernel::main {
firefly::drivers::vga::cursor cout;

firefly::drivers::vga::cursor &internal_cursor_handle() {
    return cout;
}

void write_ff_info() {
    using firefly::drivers::vga::clear;
    clear();

    firefly::kernel::main::cout << "FireflyOS\nVersion: " << VERSION_STRING << "\nContributors:";

    firefly::std::array<const char *, 3> arr = {
        "Lime\t  ", "JohnkaS", "V01D-NULL"
    };

    for (size_t i = 0; i < arr.max_size(); i++) {
        if (i % 2 == 0) {
            firefly::kernel::main::cout << "\n\t";
        }
        firefly::kernel::main::cout << arr[i] << "  ";
    }
    firefly::kernel::main::cout << "\n";
}
}  // namespace firefly::kernel::main

extern "C" [[noreturn]] void kernel_main(mboot_param magic, [[maybe_unused]] mboot_param addr) {
    using firefly::drivers::vga::color;
    using firefly::drivers::vga::cursor;

    firefly::drivers::vga::init();
    firefly::drivers::vga::cursor _cursor = { color::white, color::black, 0, 0 };
    firefly::kernel::main::cout = _cursor;
    firefly::kernel::main::write_ff_info();
    firefly::kernel::kernel_init(magic, addr);

    while (1)
        ;
}