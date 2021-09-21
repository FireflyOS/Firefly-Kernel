#include <stl/cstdlib/cstdint.h>
#include <stl/cstdlib/stdio.h>

#include <x86_64/applications/application_pointers.hpp>
#include <x86_64/applications/test/main.hpp>
#include <x86_64/applications/help/main.hpp>
#include <x86_64/memory-manager/virtual_memory.hpp>

namespace firefly::applications {
    const char *application_names[256] = {"test", "help"};
    int *application_addresses[256];
    uint16_t application_access_control[256] = {0x00, 0x00};

    #define is_avaliable application_names[temp_pointer] == application && (application_access_control[temp_pointer] < access_rights || application_access_control == access_rights)  

    void registerApplications(){
        application_addresses[0] = (int *)applications::test::test_main;
        application_addresses[1] = (int *)applications::help::help_main;

        return;
    }
    int run(const char application[], uint16_t access_rights){
        uint8_t temp_pointer = 0;
        while(temp_pointer < 255){
            if(is_avaliable) {
                int result = ((int (*)(uint8_t *))application_addresses[temp_pointer])(kernel::virtual_memory::create_virtual_memory());

                return result;
            }
            temp_pointer++;
        }
        return -1;
    }
}