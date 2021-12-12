#include <stl/cstdlib/cstdint.h>
#include <x86_64/settings.hpp>

namespace firefly::kernel::settings {
    /*
        Kernel Settings:
            1.   - Kernel Mode ('d')debug, ('p')production
            2.   - Enable Serial Port ('y')yes, ('n')no
            3.   - Disable Application Access Rights ('y')yes, ('n')no
            4.   - Disable Memory Block Access Rights ('y')yes, ('n')no
            5.   - Limit Block Count to ('h')512 or ('l')256 or ('f')1024
            6.   - Print interrupts and exceptions on serial port too. ('y')yes, ('n')no
            7-8. - Reserved
    */
    uint8_t kernel_settings[8] = {0xd5, 0x00, 0xff, 0xff, 0x14, 0x01, 0x00, 0x00};
    
    /*
    Here you can change settings of Firefly Kernel! => <dynnfy  >
    Kernel Settings:
    - 1. Kernel Mode ('d')debug, ('p')production
    - 2. Enable Serial Port ('y')yes, ('n')no
    - 3. Disable Application Access Rights ('y')yes, ('n')no
    - 4. Disable Memory Block Access Rights ('y')yes, ('n')no
    - 5. Limit Block Count to ('h')512 or ('l')256 or ('f')1024
    - 6. Print interrupts and exceptions on serial port too. ('y')yes, ('n')no
    - 7-8. Reserved
    */
    char *instruction = (char *)"              Here you can change settings of Firefly Kernel! ->    <dynnf   >\nKernel Settings:\n 1. Kernel Mode ('d')debug, ('p')production\n 2. Enable Serial Port ('y')yes, ('n')no\n 3. Disable Application Access Rights ('y')yes, ('n')no\n 4. Disable Memory Block Access Rights ('y')yes, ('n')no\n 5. Limit Block Count to ('h')512 or ('l')256 or ('f')1024\n 6. Print interrupts and exceptions on serial port too. ('y')yes, ('n')no\n 7-8. Reserved";
    /*
        Loads settings
    */
    void init_settings(){
        int starting_point = 0x45;

        if(instruction[starting_point + 0] == 'd') kernel_settings[0] = 0x9a;
        if(instruction[starting_point + 1] == 'y') kernel_settings[1] = 0xff;
        if(instruction[starting_point + 2] == 'y') kernel_settings[2] = 0xff;
        if(instruction[starting_point + 3] == 'y') kernel_settings[3] = 0xff;

        if(instruction[starting_point + 4] == 'l') kernel_settings[4] = 0x25;
        if(instruction[starting_point + 4] == 'h') kernel_settings[4] = 0x51;
        if(instruction[starting_point + 4] == 'f') kernel_settings[4] = 0x12;
        
        if(instruction[starting_point + 5] == 'y') kernel_settings[5] = 0x01;
        if(instruction[starting_point + 5] == 'n') kernel_settings[5] = 0x00;
        return;
    }
}