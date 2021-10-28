#include <stl/cstdlib/stdio.h>
#include <stl/cstdlib/cstdint.h>
#include <x86_64/applications/help/main.hpp>

namespace firefly::applications::help {
    int help_main([[maybe_unused]] int argc, [[maybe_unused]] char **argv){
        printf("--- Commands: \n\nhelp - Help command\ntest - Hello World command\nsettings - get kernel settings\nfunc-pointers - get function pointers\n");
        
        return 0;
    }

    int getc(){ return 869425; }
}
// // uint64_t res;
//             asm ("mov %0, rax\n\t"
//                  "ret"
//                     : "=r"(res)   
//                 );
//             printf("rax: 0x%X\n", res);

//             asm ("mov %0, rbx\n\t"
//                  "ret"
//                     : "=r"(res)   
//                 );
//             printf("rbx: 0x%X\n", res);

//             asm ("mov %0, rbx\n\t"
//                  "ret"
//                     : "=r"(res)   
//                 );
//             printf("rbx: 0x%X\n", res);
//             return false;