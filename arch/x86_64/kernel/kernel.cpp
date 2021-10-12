#include <stl/array.h>
#include <stl/cstdlib/stdio.h>
#include <x86_64/libk++/iostream.h>
#include <stl/cstdlib/cstddef.h>

#include <x86_64/drivers/ps2.hpp>
#include <x86_64/drivers/serial.hpp>
#include <x86_64/drivers/serial_legacy.hpp>

#include <x86_64/drivers/vbe.hpp>
#include <x86_64/kernel.hpp>
#include <x86_64/trace/strace.hpp>

#include <x86_64/applications/application_manager.hpp>

#include <x86_64/memory-manager/greenleafy.hpp>

#include <x86_64/settings.hpp>

[[maybe_unused]] constexpr short MAJOR_VERSION = 0;
[[maybe_unused]] constexpr short MINOR_VERSION = 0;
constexpr const char *VERSION_STRING = "0.0-x86_64-fork";

namespace firefly::kernel::main {
/*
    Prints Firefly's Contributors
*/
void write_ff_info() {
    printf("Firefly\nVersion: %s\nContributors:", VERSION_STRING);

    firefly::std::array<const char *, 4> arr = {
        "Lime\t  ", "JohnkaS", "V01D-NULL\t  ", "SergeyMC9730"
    };

    for (size_t i = 0; i < arr.max_size(); i++) {
        if (i % (arr.max_size() - 1) == 0) {
            puts("\n\t");
        }
        printf("%s  ", arr[i]);
    }
    puts("\n");
}

/*
    Initilizates a keyboard driver
*/
void init_keyboard(){
    printf("Initialization a Keyboard...\n");

    bool isKeyboard = firefly::drivers::ps2::init();
    io::legacy::writeTextSerial("Keyboard Driver returned %d\n\n", (isKeyboard) ? 1 : 0);
}

/*
    Kernel
*/
void kernel_main() {
    applications::registerApplications();

    write_ff_info();
    init_keyboard();                      

    printf("\n---- Memory Blocks Example ----\n\n");

    uint8_t *test_block = (uint8_t *)firefly::mm::greenleafy::alloc_block(0);
    printf("uint8_t *Test Block 1 Address: 0x%X", test_block);
    test_block[22] = 11;
    printf("uint8_t *Test Block 1[22]: 0x%X\n\n> ", test_block[22]);


    for(;;){
        firefly::drivers::ps2::handle_input();
    }

    // trace::panic(trace::PM_MANUALLYCRASHED, trace::PC_MANUALLYCRASHED);
}
}  // namespace firefly::kernel::main