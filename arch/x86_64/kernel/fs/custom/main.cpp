#include <x86_64/fs/custom/main.hpp>
#include <stl/cstdlib/cstring.h>
#include <x86_64/random.hpp>

namespace firefly::kernel::fs::custom {
    structure filesystem;
    void init(const char *name){
        uint8_t i = 0;
        while(i < 64 && name[i] != 0){
            filesystem.name[i] = name[i];
            i++;
        }
        return;
    }
    f *make_file(const char *name, int format){
        //find free file slot
        uint16_t i = 0;
        while(i < 256 && filesystem.files[i].is_used == 1) {
            if(i == 255 && filesystem.files[i].is_used == 1) return nullptr;
            i++;
        }
        //rename file
        uint16_t fslot = i;
        i = 0;
        while(i < 64 && name[i] != 0){
            filesystem.files[fslot].name[i] = name[i];
            i++;
        }
        filesystem.file_count++;
        filesystem.files[fslot].id = firefly::kernel::randomizer::rand();
        filesystem.files[fslot].format = format;
        filesystem.files[fslot].is_used = 1;
        
        return &filesystem.files[fslot]; 
    }
    void remove_file(int id){
        uint16_t i = 0;
        while(i < 256 && filesystem.files[i].id != id) {
            if(i == 255 && filesystem.files[i].id != id) return;
            i++;
        }
        filesystem.files[i].is_used = 0;

        return;
    }
}