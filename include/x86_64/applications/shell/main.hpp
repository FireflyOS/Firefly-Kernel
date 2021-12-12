#pragma once

#include <x86_64/api/api.hpp>
#include "x86_64/applications/shell/main.hpp"
#include "x86_64/drivers/vbe.hpp"
#include <x86_64/drivers/ps2.hpp>
#include <x86_64/drivers/serial_legacy.hpp>
#include "x86_64/sleep.hpp"
#include <x86_64/pmp/mp.hpp>
#include <x86_64/shell/shell.hpp>
#include <x86_64/drivers/ports.hpp>
#include <stl/cstdlib/stdio.h>

#define KEY_UP    0xC8
#define KEY_LEFT  0xCB
#define KEY_DOWN  0xD0
#define KEY_RIGHT 0xCD

#define KEY_UPL    0x48
#define KEY_LEFTL  0x4B
#define KEY_DOWNL  0x50
#define KEY_RIGHTL 0x4D

#define KEY_ENTER  0x9C

//not used
#define MOUSE_SPEED 4

namespace firefly::applications::shell {
    //mouse speed, x and y
    extern int mouse_speed, mousex, mousey;

    //function list for mouse handler
    extern mouse_handle mouse_handle_list[32];

    //layer list
    extern layer layers[32];

    //halt shell renderer for perfomance
    extern bool halt_draw;

    //current key
    extern uint8_t current_key;

    namespace test {
        //debug numbers
        extern uint8_t col0, col1, col2, col3;
    }
    
    //temp sbif
    extern sbif sbiftemp;
    
    //layers
    extern uint8_t mouse_layer_id;
    extern uint8_t start_layer_id;
    extern uint8_t test_layer_id;

    //we will disable standart keyboard driver for own shell keyboard driver
    void keyboard_handle();

    //make layer
    uint8_t make_layer(layer *l);

    //make button that mouse clickes working
    uint8_t make_mouse_handler(void (*handler)(), layer *l);
    
    //start button menu ui
    void gui_start_main();

    //search layer by name id
    uint8_t search_layer(char *nid);

    //mouse handler
    void mouse([[maybe_unused]] firefly::kernel::mp::Process *process);

    //shell renderer handler
    void layer_thread([[maybe_unused]] firefly::kernel::mp::Process *process);

    //shell ui
    void make_shell([[maybe_unused]] firefly::kernel::mp::Process *process);

    //get ready for ui
    void shell_init([[maybe_unused]] firefly::kernel::mp::Process *process);

    //entry point
    int shell_main([[maybe_unused]] int argc, [[maybe_unused]] char **argv);

    //get checksum
    int getc();
}