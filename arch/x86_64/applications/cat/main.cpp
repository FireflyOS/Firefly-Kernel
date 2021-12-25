#include <stl/cstdlib/stdio.h>
#include <x86_64/fs/custom/main.hpp>
#include <x86_64/applications/cat/main.hpp>
#include <x86_64/drivers/ps2.hpp>
#include <x86_64/sleep.hpp>
#include <x86_64/drivers/serial_legacy.hpp>

namespace firefly::applications::cat {
    char buffer[64];
    uint16_t aa = 0;
    bool k = false;
    bool strcmp_a(){
        uint8_t p = 0;
        while(p < 64){
            if(firefly::kernel::fs::custom::filesystem.files[aa].name[p] != buffer[p]) {
                return false;
            };
            p++;
        }
        return true;
    }
    uint8_t current_key;
    uint8_t buf;
    void keyboard_handle(){
        if(current_key != 0 && current_key != 10){
            buffer[buf] = current_key;
            buf++;
        }
        return;
        
    }
    int cat_main([[maybe_unused]] int argc, [[maybe_unused]] char **argv){
        uint8_t i = 0;
        buf = 0;
        current_key = 0;
        while(i < 64) buffer[i++] = 0;
        printf("Please, enter name of file:\n> ");
        firefly::drivers::ps2::redirect_to_app(keyboard_handle, &current_key);
        while(current_key != 10){
            current_key = 0;
            firefly::drivers::ps2::app_input();
        }
        aa = 0;
        while(aa < 256){
            if(strcmp_a()){
                printf("%s|%d\n@\n", firefly::kernel::fs::custom::filesystem.files[aa].data, firefly::kernel::fs::custom::filesystem.files[aa].id);       
            }
            aa++;
        }
        current_key = 0;
        buf = 0;
        i = 0;
        while(i < 64) buffer[i++] = 0;
        
        return 0;
    }

    int getc(){ return 472456; }
}