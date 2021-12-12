#pragma once

#include <x86_64/drivers/vbe.hpp>
#include <x86_64/sleep.hpp>

namespace firefly::kernel::shell::gui {
    inline void make_rectangle(int x, int y, int sizex, int sizey, int gbar_color, unsigned long long speed){
        int i = x;
        int i0 = y;
        while(i < x + sizex){ //x
            while(i0 < y + sizey){ //y
                firefly::drivers::vbe::put_pixel(i, i0, gbar_color);
                i0++;
            }
            i++;
            i0 = y;
            if(speed != 0) firefly::kernel::sleep::sleep(speed);
        }
        return;
    }

    inline void make_cube(int x, int y, int size, int gbar_color, unsigned long long speed){
        make_rectangle(x, y, size, size, gbar_color, speed);
        return;
    }
    inline void make_text(const char *text, int x, int y, int gbar_color, unsigned long long speed){
        int i = 0;
        int xx = x;
        int yy = y;
        while (text[i] != 0) {
            switch(text[i]){
                case '\n': {
                    yy += 16;
                    xx = x;
                    break;
                }
                case '\t': {
                    xx += 4;
                    break;
                }
                case '\b': {
                    xx -= 8;
                    break;
                }
                default: {
                    xx += 8;
                    break;
                }
            }
            firefly::drivers::vbe::putc(text[i++], xx, yy, gbar_color);
            if(speed != 0) firefly::kernel::sleep::sleep(speed);
        }
    }
}