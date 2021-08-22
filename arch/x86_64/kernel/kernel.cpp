#include <stl/array.h>
#include <stl/cstdlib/stdio.h>
#include <x86_64/libk++/iostream.h>

#include <x86_64/drivers/ps2.hpp>
#include <x86_64/drivers/serial.hpp>
#include <x86_64/drivers/vbe.hpp>
#include <x86_64/kernel.hpp>
#include <x86_64/trace/strace.hpp>



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


void kernel_main() {
    write_ff_info();
    for (int i = 0; i < 47; i++)
        printf("test %d\n", i);


    printf("Scrolling demo complete\n");
    trace::panic("Test");
}
}  // namespace firefly::kernel::main