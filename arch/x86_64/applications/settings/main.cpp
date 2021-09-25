#include <x86_64/settings.hpp>
#include <stl/cstdlib/stdio.h>

namespace firefly::applications::settings {
    int settings_main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {
        uint8_t kernel_mode = firefly::kernel::settings::get::kernel_mode();
        uint8_t enable_serial_port = firefly::kernel::settings::get::enable_serial_port();
        uint8_t disable_memory_block_access_rights = firefly::kernel::settings::get::disable_memory_block_access_rights();
        uint8_t disable_app_access_rights = firefly::kernel::settings::get::disable_app_access_rights();
        uint8_t block_count = firefly::kernel::settings::get::block_count();

        uint32_t real_block_count = 0;

        switch(block_count){
            case 0x12:
                real_block_count = 1024;
                break;
            case 0x51:
                real_block_count = 512;
            case 0x25:
                real_block_count = 256;
                break;
        }

        printf("Kernel Mode: 0x%X\n", kernel_mode);
        printf("Enable Serial Port: 0x%X\n", enable_serial_port);
        printf("Disable Memory block Access Rights: 0x%X\n", disable_memory_block_access_rights);
        printf("Disable Application Access Rights: 0x%X\n", disable_app_access_rights);
        printf("Max Block Count: 0x%X (%d)\n", block_count, real_block_count);

        return 0;
    };

    int getc(){ return 497; }
}