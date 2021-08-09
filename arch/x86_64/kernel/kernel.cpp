#include <cstdlib/stdio.h>
#include <stl/array.h>
#include <x86_64/libk++/ios.h>
#include <x86_64/libk++/iostream.h>

#include <x86_64/drivers/ps2.hpp>
#include <x86_64/drivers/vga.hpp>
#include <x86_64/init/init.hpp>
#include <x86_64/int/interrupt.hpp>
#include <x86_64/multiboot2.hpp>

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
    firefly::kernel::main::cout << "CursedFirefly\nVersion: " << VERSION_STRING << "\nContributors:";

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

int mprintf([[maybe_unused]] const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    [[maybe_unused]] int i = 0;
    [[maybe_unused]] int len = strlen(fmt);
    [[maybe_unused]] int res = 0;

    for (; i < len; i++) {
        switch (fmt[i]) {
            case '%': {
                switch (fmt[i + 1]) {
                    case 'c': {
                        char arg = va_arg(ap, int);
                        firefly::libkern::print((char)arg);
                        i += 2, ++res;
                        break;
                    }

                    case 's': {
                        char *arg = va_arg(ap, char *);
                        firefly::libkern::print(arg);
                        i += 2, (res += 2 + strlen(arg));
                        break;
                    }

                    // case 'i':
                    case 'd': {
                        int arg = va_arg(ap, int);
                        char buff[20];
                        firefly::libkern::print(itoa(arg, buff, 10));
                        res += digitcount(arg);
                        i += 2;
                        break;
                    }

                    case 'x': {
                        int arg = va_arg(ap, int);
                        char buff[20];
                        firefly::libkern::print(itoa(arg, buff, 16));
                        res += strlen(buff);
                        i += 2;
                        break;
                    }
                        // case 'X': {
                        //     int arg = va_arg(ap, int);
                        //     char buff[20];
                        //     firefly::libkern::print(itoa(arg, buff, 16, true));
                        //     res += strlen(buff);
                        //     i += 2;
                        //     break;
                        // }

                        // case 'b': {
                        //     int arg = va_arg(ap, int);
                        //     char buff[20];
                        //     firefly::libkern::print("0b", itoa(arg, buff, 2));
                        //     i += 2;
                        //     break;
                        // }

                        // case 'o': {
                        //     // int arg = va_arg(ap, int);
                        //     // char buff[20];
                        //     // firefly::libkern::print(itoa(arg, buff, 8));
                        //     i += 2;
                        //     break;
                        // }
                }
            }
            default:
                firefly::libkern::print(fmt[i]);
                va_end(ap);
                res++;
                break;
        }
    }
    return res;
}

extern "C" [[noreturn]] void kernel_main(uint64_t magic, uint64_t mb2_proto_struct) {
    using firefly::drivers::vga::color;
    using firefly::drivers::vga::cursor;

    firefly::drivers::vga::init();
    firefly::kernel::main::cout = { color::white, color::black, 0, 0 };
    firefly::kernel::main::write_ff_info();
    firefly::drivers::ps2::init();
    firefly::kernel::core::interrupt::init();
    firefly::kernel::kernel_init(magic, mb2_proto_struct);

    //Printf test
    int res = printf("Hex: %x\n", 0xabc);
    printf("%d chars were written\n", res);
    printf("Address of res is: %X\n", &res);
    printf("octal: %o\n", 100);
    printf("long hex: 0x%x\n", 0xabcdef12);

    firefly::kernel::core::interrupt::test_int();

    printf("Post interrupt");

    while (true) {
        auto key = firefly::drivers::ps2::get_scancode();
        if (!key.has_value()) {
            continue;
        }
        firefly::drivers::ps2::handle_input(*key, firefly::libkern::get_cursor_handle());
    }
}
