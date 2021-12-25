#include <stl/cstdlib/stdio.h>
#include <x86_64/fs/custom/main.hpp>
#include <x86_64/applications/ls/main.hpp>

namespace firefly::applications::ls {
    int ls_main([[maybe_unused]] int argc, [[maybe_unused]] char **argv){
        int i = 0;
        int total_size = 0;
        while(i < 256){
            total_size += firefly::kernel::fs::custom::filesystem.files[i].size;
            i++;
        }
        i = 0;
        printf("Volume \"%s\" : %d bytes in total.\n", firefly::kernel::fs::custom::filesystem.name, total_size);
        while(i < 256){
            if(firefly::kernel::fs::custom::filesystem.files[i].is_used == 1){
                printf(": %s@%d -> %d bytes\n", firefly::kernel::fs::custom::filesystem.files[i].name, firefly::kernel::fs::custom::filesystem.files[i].id, firefly::kernel::fs::custom::filesystem.files[i].size);
            }
            i++;
        }
        return 0;
    }

    int getc(){ return 422529; }
}