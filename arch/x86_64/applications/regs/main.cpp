#include <stl/cstdlib/stdio.h>
#include <stl/cstdlib/cstdint.h>
#include <x86_64/applications/regs/main.hpp>
#include <x86_64/drivers/ps2.hpp>

namespace firefly::applications::regs {
    uint8_t current_key;
    void keyboard_handle(){
        printf("keyboard_handle was triggered\n");
        printf("%c\n", current_key);
    }
    
    int regs_main([[maybe_unused]] int argc, [[maybe_unused]] char **argv){
        drivers::ps2::redirect_to_app(&keyboard_handle, &current_key);
        while(1){
            drivers::ps2::app_input();
        }
        //     uint64_t res;
        //     asm ("mov %0, rax\n\t"
        //         "ret"
        //             : "=r"(res)   
        //         );
        //     printf("rax: 0x%X\n", res);

        //     asm ("mov %0, rbx\n\t"
        //         "ret"
        //             : "=r"(res)   
        //         );
        //     printf("rbx: 0x%X\n", res);

        //     asm ("mov %0, rbx\n\t"
        //         "ret"
        //             : "=r"(res)   
        //     );
        // printf("rbx: 0x%X\n", res);
        
        return 0;
    }

    int getc(){ return 869453; }
}