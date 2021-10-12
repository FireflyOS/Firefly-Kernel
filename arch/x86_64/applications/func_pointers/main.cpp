#include <stl/cstdlib/stdio.h>
#include <x86_64/drivers/serial_legacy.hpp>
#include <x86_64/applications/func_pointers/main.hpp>

namespace firefly::applications::func_pointers {
    int func_pointers_main([[maybe_unused]] int argc, [[maybe_unused]] char **argv){
        printf("printf: 0x%X (const char *)\n", &printf);
        printf("firefly::kerne::io::legacy::writeTextSerial: 0x%X (const char *)\n", &firefly::kernel::io::legacy::writeTextSerial);
        return 0;
    }

    int getc(){ return 4217513; }
}