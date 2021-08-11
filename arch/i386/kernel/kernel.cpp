#include <i386/libk++/ios.h>
#include <i386/libk++/iostream.h>
#include <stdint.h>
#include <stl/array.h>
#include <stl/cstdlib/stdio.h>

#include <i386/drivers/vga.hpp>
#include <i386/gdt/gdt.hpp>
#include <i386/init/init.hpp>
#include <i386/int/interrupt.hpp>
#include <i386/multiboot2.hpp>
#include <i386/drivers/serial.hpp>

[[maybe_unused]] constexpr short MAJOR_VERSION = 0;
[[maybe_unused]] constexpr short MINOR_VERSION = 0;
constexpr const char *VERSION_STRING = "0.0";

namespace firefly::kernel::main {
// firefly::drivers::vga::cursor cout;

void write_ff_info() {
    using firefly::drivers::vga::clear;
    clear();

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

firefly::kernel::main::Singleton firefly::kernel::main::Singleton::instance;
extern "C" [[noreturn]] void kernel_main([[maybe_unused]] mboot_param magic, [[maybe_unused]] mboot_param addr) {
    using firefly::drivers::vga::color;
    using firefly::drivers::vga::cursor;

    firefly::kernel::core::gdt::init();
    firefly::drivers::vga::init();
    firefly::drivers::vga::cursor _cursor = { color::white, color::black, 0, 0 };
    firefly::kernel::main::Singleton::Get().internal_set_cursor_handle(_cursor);
    firefly::kernel::main::write_ff_info();
    firefly::kernel::core::interrupt::init();
    firefly::kernel::kernel_init(magic, addr);

    firefly::kernel::io::SerialPort port = { firefly::kernel::io::SerialPort::COM1, firefly::kernel::io::SerialPort::BAUD_BASE };
    port.initialize();

    port.send_chars("hello", 6);
    char buff[10];
    port.read_string(buff, 10);
    klog() << "Serial port received: " << buff << '\n';
    // firefly::kernel::core::interrupt::test_int();

    //Printf test
    int res = printf("Hex: %x\n", 0xabc);
    printf("%d chars were written\n", res);
    printf("Address of res is: %X\n", &res);
    printf("octal: %o\n", 100);
    printf("long hex: 0x%x\n", 0xabcdef12);

    while (1)
        ;
}