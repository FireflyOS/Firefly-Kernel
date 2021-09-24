#include <stl/cstdlib/cstdint.h>
#include <stl/cstdlib/stdio.h>

#include <x86_64/applications/application_pointers.hpp>
#include <x86_64/drivers/serial_legacy.hpp>
#include <x86_64/applications/test/main.hpp>
#include <x86_64/applications/help/main.hpp>

namespace firefly::applications {
    const char *application_names[256] = {"test", "help"};
    int *application_addresses[256];
    uint16_t *application_access_control[256] = {0x0000, 0x0000};

    void registerApplications(){
        application_addresses[0] = (int *)applications::test::test_main;
        firefly::kernel::io::legacy::writeTextSerial("Registered Test Command on address 0x%X\n\n", &applications::test::test_main);
        application_addresses[1] = (int *)applications::help::help_main;
        firefly::kernel::io::legacy::writeTextSerial("Registered Help Command on address 0x%X\n\n", &applications::help::help_main);

        return;
    }

    int run(const char application[], uint16_t *access_rights, int argc, char **argv){
        uint8_t temp_pointer = 0;
        while(temp_pointer < 255){
            if(application_names[temp_pointer] == application && ((uint16_t *)application_access_control[temp_pointer] <= (uint16_t *)access_rights || application_access_control[temp_pointer] == access_rights)) {
                int result = ((int (*)(int, char **))application_addresses[temp_pointer])(argc, argv);

                return result;
            }
            temp_pointer++;
        }
        return -1;
    }

    const char *get_commands(){
        return *application_names;
    }
}