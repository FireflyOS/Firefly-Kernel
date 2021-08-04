#include <cstdlib/stdio.h>
#include <stl/array.h>

#include <drivers/ps2.hpp>
#include <drivers/vga.hpp>
#include <init/init.hpp>
#include <int/interrupt.hpp>
#include <multiboot2.hpp>

[[maybe_unused]] constexpr short MAJOR_VERSION = 0;
[[maybe_unused]] constexpr short MINOR_VERSION = 0;
constexpr const char* VERSION_STRING = "0.0";

void write_ff_info(firefly::drivers::vga::cursor& crs) {
    using firefly::drivers::vga::clear;
    clear();

    crs << "FireflyOS\nVersion: " << VERSION_STRING << "\nContributors:";

    firefly::std::array<const char*, 3> arr = {
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

extern "C" [[noreturn]] void kernel_main(uint64_t mb2_proto_struct) {
    using firefly::drivers::vga::color;
    using firefly::drivers::vga::cursor;

    firefly::kernel::kernel_init(mb2_proto_struct);
    firefly::drivers::vga::init();
    firefly::drivers::ps2::init();
    firefly::kernel::interrupt::init();

    cursor crs{ color::white, color::black, 0, 0 };

    write_ff_info(crs);

    char abc[10];
    crs << "Testing " << itoa(123, abc, 10) << "\n";

    // eh
    firefly::kernel::start_load(crs, "Loading VGA driver");
    firefly::kernel::end_load(crs, "Loaded VGA driver");
    firefly::kernel::start_load(crs, "Loading PS/2 driver");
    firefly::kernel::end_load(crs, "Loaded PS/2 driver");

    while (true) {
        auto key = firefly::drivers::ps2::get_scancode();
        if (!key.has_value()) {
            continue;
        }
        firefly::drivers::ps2::handle_input(*key, crs);
    }
}
