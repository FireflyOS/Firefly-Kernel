#include <stl/cstdlib/stdio.h>
#include <stl/cstdlib/cstdint.h>
#include <x86_64/applications/help/main.hpp>

namespace firefly::applications::help {
    int help_main([[maybe_unused]] int argc, [[maybe_unused]] char **argv){
        printf("--- Commands: \n\nhelp - Help command\ntest - Hello World command\nsettings - get kernel settings\nshell - start the shell\nregs - test keyboard redirecting\n");
        
        return 0;
    }

    int getc(){ return 869425; }
}