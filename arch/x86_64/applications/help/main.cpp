#include <stl/cstdlib/stdio.h>
#include <stl/cstdlib/cstdint.h>
#include <x86_64/applications/help/main.hpp>

namespace firefly::applications::help {
    int help_main(uint8_t *virtual_memory_instance){
        printf("Commands: \n\nhelp - Help command\ntest - Hello World command\ncmdlist - command list\n");
        virtual_memory_instance[0]++;
        return 0;
    }
}