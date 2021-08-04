#include <cstdlib/stdio.h>
#include <stl/array.h>
#include <stl/ios.h>
#include <stl/iostream.h>

#include <drivers/ps2.hpp>
#include <drivers/vga.hpp>
#include <init/init.hpp>
#include <int/interrupt.hpp>
#include <multiboot2.hpp>

[[maybe_unused]] constexpr short MAJOR_VERSION = 0;
[[maybe_unused]] constexpr short MINOR_VERSION = 0;
constexpr const char *VERSION_STRING = "0.0";

namespace firefly::kernel::main {
void write_ff_info() {
    using firefly::drivers::vga::clear;
    clear();

    firefly::std::cout << "FireflyOS\nVersion: " << VERSION_STRING << "\nContributors:";

    firefly::std::array<const char *, 3> arr = {
        "Lime\t  ", "JohnkaS", "V01D-NULL"
    };

    for (size_t i = 0; i < arr.max_size(); i++) {
        if (i % 2 == 0) {
            firefly::std::cout << "\n\t";
        }
        firefly::std::cout << arr[i] << "  ";
    }
    firefly::std::cout << "\n";
}
}  // namespace firefly::kernel::main

extern "C" [[noreturn]] void kernel_main(uint64_t *mb2_proto_struct) {
    using firefly::drivers::vga::color;
    using firefly::drivers::vga::cursor;

    firefly::drivers::vga::init();
    firefly::std::globalize_vga_writer();
    firefly::kernel::main::write_ff_info();
    firefly::drivers::ps2::init();
    firefly::kernel::interrupt::init();
    firefly::kernel::kernel_init(mb2_proto_struct);

    firefly::std::cout << "Hello world! This is a number: " << 534982 << firefly::std::endl;
    firefly::std::cout << firefly::std::hex(0xF123) << firefly::std::endl;

    while (true) {
        auto key = firefly::drivers::ps2::get_scancode();
        if (!key.has_value()) {
            continue;
        }
        firefly::drivers::ps2::handle_input(*key, firefly::std::get_cursor_handle());
    }
}
