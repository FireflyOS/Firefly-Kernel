#include <cstdlib/stdio.h>
#include <stl/array.h>

#include <drivers/ps2.hpp>
#include <drivers/vga.hpp>
#include <init/init.hpp>
#include <int/interrupt.hpp>
#include <multiboot2.hpp>

[[maybe_unused]] constexpr short MAJOR_VERSION = 0;
[[maybe_unused]] constexpr short MINOR_VERSION = 0;
constexpr const char *VERSION_STRING = "0.0";

namespace firefly::kernel::main {
/* This allows the kernel to get a global reference to the cursor object */
firefly::drivers::vga::cursor cur;
firefly::drivers::vga::cursor &get_cursor() {
    return cur;
}

void globalize_vga_writer() {
    drivers::vga::cursor crs = { drivers::vga::color::white, drivers::vga::color::black, 0, 0};
    cur = crs;
}

void write_ff_info() {
    using firefly::drivers::vga::clear;
    firefly::drivers::vga::cursor &crs = get_cursor();
    clear();

    crs << "FireflyOS\nVersion: " << VERSION_STRING << "\nContributors:";

    firefly::std::array<const char *, 3> arr = {
        "Lime\t  ", "JohnkaS", "V01D-NULL"
    };

    for (size_t i = 0; i < arr.max_size(); i++) {
        if (i % 2 == 0) {
            crs << "\n\t";
        }
        crs << arr[i] << "  ";
    }
    crs << "\n";
}
}  // namespace firefly::kernel::main

extern "C" [[noreturn]] void kernel_main(uint64_t mb2_proto_struct) {
    using firefly::drivers::vga::color;
    using firefly::drivers::vga::cursor;

    firefly::drivers::vga::init();
    firefly::kernel::main::globalize_vga_writer();
    firefly::kernel::main::write_ff_info();

    firefly::drivers::ps2::init();
    firefly::kernel::interrupt::init();
    firefly::kernel::kernel_init(mb2_proto_struct);

    // crs << "Testing " << 10 << "\n";

    // eh
    firefly::kernel::start_load("Loading VGA driver");
    firefly::kernel::end_load("Loaded VGA driver");
    firefly::kernel::start_load("Loading PS/2 driver");
    firefly::kernel::end_load("Loaded PS/2 driver");

    while (true) {
        auto key = firefly::drivers::ps2::get_scancode();
        if (!key.has_value()) {
            continue;
        }
        firefly::drivers::ps2::handle_input(*key, firefly::kernel::main::get_cursor());
    }
}
