#include <i386/libk++/ios.h>
#include <i386/libk++/iostream.h>
#include <stdint.h>
#include <stl/array.h>
#include <stl/cstdlib/stdio.h>

#include <i386/drivers/serial.hpp>
#include <i386/drivers/vbe.hpp>
#include <i386/drivers/vga.hpp>
#include <i386/gdt/gdt.hpp>
#include <i386/init/init.hpp>
#include <i386/int/interrupt.hpp>
#include <i386/multiboot2.hpp>
#include <i386/utils.hpp>

[[maybe_unused]] constexpr short MAJOR_VERSION = 0;
[[maybe_unused]] constexpr short MINOR_VERSION = 0;
constexpr const char *VERSION_STRING = "0.0";

namespace firefly::kernel::main {
void write_ff_info() {
    printf("FireflyOS\nVersion: %s\nContributors:", VERSION_STRING);

    firefly::std::array<const char *, 3> arr = {
        "Lime\t  ", "JohnkaS", "V01D-NULL"
    };

    for (size_t i = 0; i < arr.max_size(); i++) {
        if (i % 2 == 0) {
            puts("\n\t");
        }
        printf("%s  ", arr[i]);
    }
    puts("\n");
}
}  // namespace firefly::kernel::main

#include <splash.h>
extern "C" [[noreturn]] void kernel_main(mboot_param magic, mboot_param addr) {
    firefly::kernel::core::gdt::init();
    firefly::kernel::core::interrupt::init();
    firefly::kernel::kernel_init(magic, addr);
    firefly::drivers::vbe::boot_splash();
    
    while (1)
        ;
}