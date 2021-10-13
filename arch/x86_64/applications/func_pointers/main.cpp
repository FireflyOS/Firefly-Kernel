#include <stl/cstdlib/stdio.h>
#include <x86_64/applications/func_pointers/main.hpp>
#include <x86_64/drivers/serial_legacy.hpp>
#include <x86_64/memory-manager/greenleafy.hpp>

namespace firefly::applications::func_pointers {
    int func_pointers_main([[maybe_unused]] int argc, [[maybe_unused]] char **argv){
        printf("int printf: 0x%X (const char *, ...)\n", &printf);
        printf("void firefly::kerne::io::legacy::writeTextSerial: 0x%X (const char *, ...)\n\n", &firefly::kernel::io::legacy::writeTextSerial);
        printf("void *(struct)firefly::mm::greenleafy::use_block: 0x%X (uint32_t)\n", &firefly::mm::greenleafy::use_block);
        printf("void *(struct)firefly::mm::greenleafy::get_block: 0x%X (uint64_t, uint32_t)\n", &firefly::mm::greenleafy::get_block);
        printf("uint32_t firefly::mm::greenleafy::get_block_limit: 0x%X\n", &firefly::mm::greenleafy::get_block_limit);
        printf("uint32_t firefly:y:mm::greenleafy::get_block_size_limit: 0x%X\n", &firefly::mm::greenleafy::get_block_size_limit);
        return 0;
    }

    int getc(){ return 4217513; }
}