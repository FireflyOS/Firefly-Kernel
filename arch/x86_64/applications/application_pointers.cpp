#include <stl/cstdlib/cstdint.h>
#include <stl/cstdlib/stdio.h>

#include <x86_64/applications/application_pointers.hpp>
#include <x86_64/applications/test/main.hpp>
#include <x86_64/applications/help/main.hpp>
#include <x86_64/memory-manager/virtual_memory.hpp>

namespace firefly::applications {
    const char *application_names[256] = {"test", "help"};
    int *application_addresses[256];  

    void registerApplications(){
        application_addresses[0] = (int *)applications::test::test_main;
        application_addresses[1] = (int *)applications::help::help_main;

        return;
    }
    int run(const char application[]){
        uint8_t temp_pointer = 0;
        while(temp_pointer < 255){
            if(application_names[temp_pointer] == application) {
                int result = ((int (*)(uint8_t *))application_addresses[temp_pointer])(kernel::virtual_memory::create_virtual_memory());

                return result;
            }
            temp_pointer++;
        }
        return -1;
    }
}