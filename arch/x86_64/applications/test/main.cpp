#include <stl/cstdlib/stdio.h>
#include <stl/cstdlib/cstdint.h>
#include <x86_64/applications/test/main.hpp>
#include <x86_64/api/api.hpp>

namespace firefly::applications::test {
    int test_main([[maybe_unused]] int argc, char **argv){
        FAPI::stdio::printf("Hello, API!\n");
        FAPI::kernel::io::legacy::writeTextSerial("Hello, API!\n");
        printf("Hello World!\nargv[0] - %s\nargv[1] - %s\n", argv[0], argv[1]);
        return 0;
    }

    int getc(){ return 869569; }
}