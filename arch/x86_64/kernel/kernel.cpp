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
constexpr const char *VERSION_STRING = "0.0 FORK";

namespace firefly::kernel::main {
void write_ff_info() {
    printf("Firefly\nVersion: %s\nContributors:", VERSION_STRING);

    firefly::std::array<const char *, 3> arr = {
        "Lime\t  ", "JohnkaS", "V01D-NULL\t  "
    };

    for (size_t i = 0; i < arr.max_size(); i++) {
        if (i % (arr.max_size() - 1) == 0) {
            puts("\n\t");
        }
        printf("%s  ", arr[i]);
    }
    puts("\n");
}


void kernel_main() {
    write_ff_info();
    printf("Initialization a Keyboard...\n");
    bool isKeyboard = false;
    isKeyboard = firefly::drivers::ps2::init();
    printf("Returned a \"%s\" result\n", isKeyboard ? "true" : "false");
    if(!isKeyboard) trace::panic(trace::PM_DRIVERERROR_PK, trace::PC_DRIVERERROR_PK);
    trace::panic(trace::PM_MANUALLYCRASHED, trace::PC_MANUALLYCRASHED);

    for(;;){
        unsigned char scancode = firefly::drivers::ps2::get_scancode();
        if(scancode) firefly::drivers::ps2::handle_input(scancode);
    }
}
}  // namespace firefly::kernel::main