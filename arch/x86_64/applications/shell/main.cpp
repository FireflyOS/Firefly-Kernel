#include "x86_64/applications/shell/main.hpp"
#include <x86_64/drivers/pci/ide.hpp>

#define inb firefly::kernel::io::inb

namespace firefly::applications::shell {
    int mousex, mousey;
    int mouse_speed = 4;
    mouse_handle mouse_handle_list[32];
    layer layers[32];
    bool halt_draw = false;
    uint8_t current_key;
    void keyboard_handle(){
        firefly::kernel::io::legacy::writeTextSerial("oh\n");
        return;
    }
    sbif sbiftemp;

    uint8_t mouse_layer_id;
    uint8_t start_layer_id;
    uint8_t test_layer_id;

    namespace test {
        uint8_t col0;
        uint8_t col1;
        uint8_t col2;
        uint8_t col3;
    }
    
    uint8_t make_layer(layer *l);

    uint8_t make_mouse_handler(void (*handler)(), layer *l){
        uint8_t i = 0;
        while(mouse_handle_list[i].is_used == 1) i++;
        i = (i == 1) ? 1 : i;

        mouse_handle_list[i].is_used = true;
        mouse_handle_list[i].handler = handler;
        mouse_handle_list[i].l = l;
        
        return i;
    }
    void gui_start_main(){
        return;
    }
    uint8_t make_layer(layer *l){
        halt_draw = false;
        uint8_t i = 0;

        while(layers[i].is_used == 1) {
            if(i == 32) i = 0;
            ++i;
        };

        layers[i].action = l->action;
        layers[i].gbar_color = l->gbar_color;
        layers[i].is_used = 1;
        layers[i].size = l->size;
        layers[i].sizex = l->sizex;
        layers[i].sizey = l->sizey;
        layers[i].speed = l->speed;
        layers[i].draw = l->draw;
        layers[i].x = l->x;
        layers[i].y = l->y;
        
        layers[i].text = l->text;
        layers[i].nid = l->nid;

        return i;
    }
    uint8_t search_layer(char *nid){
        uint8_t i = 0;

        while(strcmp(layers[i].nid, nid) != 0) {
            if(i == 32) return 0xFF;
            i++;
        };

        return i;
    }
    void mouse([[maybe_unused]] firefly::kernel::mp::Process *process){
        layers[mouse_layer_id].gbar_color = firefly::drivers::vbe::get_pixel(layers[mouse_layer_id].x, layers[mouse_layer_id].y + 8) + 32; 
        uint8_t scancode;

        if ((inb(100) & 1)) {
            halt_draw = false;
            scancode = inb(0x60);
            //#define DEBUG_MODE
            #ifdef DEBUG_MODE
            firefly::kernel::io::legacy::writeTextSerial("scancode: 0x%X\n", scancode);
            #endif
            switch(scancode){
                case KEY_UP: {
                    mousey += mouse_speed;
                    break;
                }
                case KEY_DOWN: {
                    mousey -= mouse_speed;
                    break;
                }
                case KEY_RIGHT: {
                    mousex += mouse_speed;
                    break;
                }
                case KEY_LEFT: {
                    mousex -= mouse_speed;
                    break;
                }

                case KEY_UPL: {
                    mousey += mouse_speed;
                    break;
                }
                case KEY_DOWNL: {
                    mousey -= mouse_speed;
                    break;
                }
                case KEY_RIGHTL: {
                    mousex += mouse_speed;
                    break;
                }
                case KEY_LEFTL: {
                    mousex -= mouse_speed;
                    break;
                }

                case KEY_ENTER: {
                    uint8_t i = 0;
                    while(mouse_handle_list[i].is_used == false) i++;

                    if(firefly::kernel::shell::l1_lays_on_l2(&layers[mouse_layer_id], mouse_handle_list[i].l))
                        mouse_handle_list[i].handler();
                }
            }
        }
        
        layers[mouse_layer_id].x = mousex;
        layers[mouse_layer_id].y = mousey;

        if(firefly::kernel::shell::l1_lays_on_l2(&layers[mouse_layer_id], &layers[start_layer_id])){
            layers[start_layer_id].gbar_color += 16;
        }
    }
    void layer_thread([[maybe_unused]] firefly::kernel::mp::Process *process){
        if(halt_draw) return;
        uint8_t i = 0;
        while(i < 32){
            if(firefly::kernel::shell::xydtest(&layers[i])){
                switch(layers[i].action){
                    case 0: { //pixel
                        firefly::drivers::vbe::put_pixel(layers[i].x, layers[i].y, layers[i].gbar_color);
                        break;
                    }
                    case 1: { //cube
                        firefly::kernel::shell::gui::make_cube(layers[i].x, layers[i].y, layers[i].size, layers[i].gbar_color, layers[i].speed);
                        break;
                    }
                    case 2: { //rectangle
                        firefly::kernel::shell::gui::make_rectangle(layers[i].x, layers[i].y, layers[i].sizex, layers[i].sizey, layers[i].gbar_color, layers[i].speed);
                        break;
                    }
                    case 3: { //text
                        firefly::kernel::shell::gui::make_text(layers[i].text, layers[i].x, layers[i].y, layers[i].gbar_color, layers[i].speed);
                        break;
                    }

                    default: while(true);
                }
            }
            
            i++;
        }
        halt_draw = true;
    }

    void make_shell([[maybe_unused]] firefly::kernel::mp::Process *process){
        [[maybe_unused]] unsigned long long speed = static_cast<unsigned long long>(process->block->block[0]);
        //turn off keyboard
        firefly::drivers::ps2::redirect_to_app(&keyboard_handle, &current_key);

        //set shell mode
        firefly::drivers::vbe::set_shell();

        mousex = mousey = 0;
        return;
    }
    void shell_init([[maybe_unused]] firefly::kernel::mp::Process *process){
        
        layer l0 = {2, 0, 0, 0, 1024, 740, 0xFFFFFFFF, 0, nullptr, nullptr, 1, 1, (char *)"syslayer_bg"};
        make_layer(&l0);

        layer l1 = {2, 68, 743, 0, 995, 25, firefly::kernel::shell::GBAR(132, 245, 118, 255, 0), 0, nullptr, nullptr, 1, 1, (char*)"syslayer_taskbar"};
        make_layer(&l1);

        layer l2 = {2, 0, 743, 0, 65, 25, firefly::kernel::shell::GBAR(118, 188, 245, 255, 0), 0, nullptr, nullptr, 1, 1, (char*)"syslayer_startb"};
        start_layer_id = make_layer(&l2);
        make_mouse_handler(&gui_start_main, &l2);

        layer l3 = {1, 0, 0, 16, 0, 0, firefly::kernel::shell::GBAR(118, 188, 245, 255, 0), 0, nullptr, nullptr, 1, 1, (char*)"syslayer_mouse"};
        mouse_layer_id = make_layer(&l3);

        layer l4 = {3, 3, 747, 0, 0, 0, firefly::kernel::shell::GBAR(46, 46, 46, 0, 0), 0, (char*)"Start", nullptr, 1, 1, (char*)"syslayer_startb_label"};
        make_layer(&l4);

        firefly::kernel::mp::make(&make_shell, 1, (char *)"tmp_shellui");
        firefly::kernel::mp::make(&mouse, 0, (char *)"mouse_thread");
        firefly::kernel::mp::make(&layer_thread, 0, (char *)"shell_renderer");
    }

    int shell_main([[maybe_unused]] int argc, [[maybe_unused]] char **argv){
        //move to pseudo multiprocessing
        firefly::kernel::mp::make(&shell_init, 1, (char *)"shell_init");

        // char buffer[16];
        // char buffer1[16];
        // char buffer2[16];
        // char buffer3[16];
        // layer l5 = {3, 10, 10, 0, 0, 0, firefly::kernel::shell::GBAR(46, 46, 46, 0, 0), 0, itoa(test::col0, buffer, 10), 1, 1};
        // layer l6 = {3, 10, 26, 0, 0, 0, firefly::kernel::shell::GBAR(46, 46, 46, 0, 0), 0, itoa(test::col1, buffer1, 10), 1, 1};
        // layer l7 = {3, 10, 42, 0, 0, 0, firefly::kernel::shell::GBAR(46, 46, 46, 0, 0), 0, itoa(test::col2, buffer2, 10), 1, 1};
        // layer l8 = {3, 10, 58, 0, 0, 0, firefly::kernel::shell::GBAR(46, 46, 46, 0, 0), 0, itoa(test::col3, buffer3, 10), 1, 1};
        // layer l9 = {1, 100, 100, 32, 0, 0, firefly::kernel::shell::GBAR(test::col0, test::col1, test::col2, test::col3, 0), 0, nullptr, 1, 1};
        // make_layer(&l5);
        // make_layer(&l6);
        // make_layer(&l7);
        // make_layer(&l8);
        // make_layer(&l9);

        firefly::kernel::mp::run();
        
        return 0;
    }

    int getc(){ return 1149725; }
}