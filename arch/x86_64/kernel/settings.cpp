#include <stl/cstdlib/cstdint.h>
#include <x86_64/settings.hpp>

namespace firefly::kernel::settings {
    /*
        Kernel Settings:
            1.   - Kernel Mode ('d'ebug, 'p'roduction)
            2.   - Enable Serial Port ('y'es, 'n'o)
            3.   - Disable Application Access Rights ('y'es, 'n'o)
            4.   - Disable Memory Block Access Rights ('y'es, 'n'o)
            5.   - Limit Block Count to ('h')512 or ('l')256 or ('f')1024
            6-8. - Reserved
    */
    uint8_t kernel_settings[9] = {0xd5, 0x00, 0xff, 0xff, 0x14, 0x00, 0x00, 0x00,   0x00};
    
    /*
        Loads settings
    */
    void init_settings(){
        if(kernel_settings[8] == 0xff) return;

        const char *instruction = "              Here you can change settings of Firefly Kernel! ->    <pynnf   >\nKernel Settings:\n 1. Kernel Mode ('d'ebug, 'p'roduction)\n 2. Enable Serial Port ('y'es, 'n'o)\n 3. Disable Application Access Rights ('y'es, 'n'o)\n 4. Disable Memory Block Access Rights ('y'es, 'n'o)\n 5. Limit Block Count to ('h')512 or ('l')256 or ('f')1024\n 6-8. Reserved";
        int start_point = 0x45;

        if(instruction[start_point + 0] == 'd') kernel_settings[0] = 0x9a;
        if(instruction[start_point + 1] == 'y') kernel_settings[1] = 0xff;
        if(instruction[start_point + 2] == 'y') kernel_settings[2] = 0xff;
        if(instruction[start_point + 3] == 'y') kernel_settings[3] = 0xff;

        if(instruction[start_point + 4] == 'l') kernel_settings[4] = 0x25;
        if(instruction[start_point + 4] == 'h') kernel_settings[4] = 0x51;
        if(instruction[start_point + 4] == 'f') kernel_settings[4] = 0x12;
        
        kernel_settings[8] = 0xff;
        return;
    }

    namespace get {
        uint8_t kernel_mode(){ return kernel_settings[0]; }
        uint8_t enable_serial_port(){ return kernel_settings[1]; }
        uint8_t disable_app_access_rights(){ return kernel_settings[2]; }
        uint8_t disable_memory_block_access_rights(){ return kernel_settings[3]; }
        uint8_t block_count(){ return kernel_settings[4]; }
    }
}