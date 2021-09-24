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

#include <x86_64/applications/application_pointers.hpp>

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
    printf("uint8_t *Test Block 1[22]: 0x%X\n", test_block[22]);

    uint8_t *test_block2 = (uint8_t *)firefly::mm::greenleafy::alloc_block(0);
    printf("uint8_t *Test Block 2 Address: 0x%X", test_block2);
    test_block2[11] = 22;
    printf("uint8_t *Test Block 2[11]: 0x%X\n", test_block2[11]);

    const char *test_block3 = (char *)firefly::mm::greenleafy::alloc_block(0);
    test_block3 = "gg ggss11121212";
    printf("const char *Test Block 3 Address: 0x%X", test_block3);
    printf("const char *Test Block 3: %s\n", test_block3);

    int *test_block4 = (int *)firefly::mm::greenleafy::alloc_block(0);
    *test_block4 = 81118;
    printf("int *Test Block 4 Address: 0x%X", test_block4);
    printf("int *Test Block 4: %d\n", *test_block4);

    const char *arguments[2] = {"test", "123"}; 

    printf("\n---- Settings Command (new) ----\n\n");
    applications::run("settings", 0x0000, (char **)arguments);

    printf("\n---- Changelog ----\n\n");
    printf("1. New NO_BLOCKS_AVALIABLE crash error\n");
    printf("2. Max Block Count and Max Block Size are configureable now!\n");
    printf("3. Firefly no more calls Panic error if keyboard is not accessable!\n");
    printf("4. In ISO file you can configure kernel settings! (Search: \"Here you can\")\n");
    printf("4.1. Documentation is included\n");
    printf("5. Some of source files has some kind of description for each variable (struct, function, etc.) now\n");
    printf("6. Added \"#pragma once\" to the Help Application header file\n");
    printf("7. Applications uses arguments like {\"test\", \"123\"} now!\n");
    printf("8. Block Access Rights are using uint32_t instead of uint64_t now\n");

    // trace::panic(trace::PM_MANUALLYCRASHED, trace::PC_MANUALLYCRASHED);
}
}  // namespace firefly::kernel::main