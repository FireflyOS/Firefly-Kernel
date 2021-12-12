#pragma once

#include "color.hpp"
#include "gui/gui.hpp"
#include <x86_64/pmp/mp.hpp>
#include <x86_64/applications/shell/etc.hpp>

struct layer {
    uint8_t action; //image, cube, rectangle or text
    int x; //image, cube, rectangle, text
    int y; //image, cube, rectangle, text
    int size; //cube
    int sizex; //rectangle
    int sizey; //rectangle
    int gbar_color; //pixel, cube, rectangle, text
    unsigned long long speed; //cube, rectangle, text
    char *text; //text
    sbif *sbif;

    uint8_t is_used;
    uint8_t draw;
    char *nid;
};
struct mouse_handle {
    void (*handler)();
    bool is_used;
    layer *l;
};

namespace firefly::kernel::shell {
    inline bool xydtest(layer *l){
        if(l->x < 0 || l->y < 0) return false;
        if(l->draw == 0) return false; 
        return true;
    }
    inline bool l1_lays_on_l2(layer *l1, layer *l2){
        if(((l1->x - l2->x) - (l1->y - l2->y)) < 0) return true;
        return false;
    }
    inline sbif *raw2sbif(int *raw, int size, int pixels){
        sbif_main a[size];
        int i = 0, i2 = 0;
        while(i < size){
            a[i2].x = raw[i + 0];
            a[i2].y = raw[i + 1];
            a[i2].gbar_color = raw[i + 2];
            i2++;
            i += 4;
        }
        firefly::applications::shell::etc::sbiftemp.sbif = a;
        firefly::applications::shell::etc::sbiftemp.size = pixels;
        return nullptr;
    }
    // inline bool strcmp(char *a, char *b, int size){
    //     int i = 0;
    //     bool s = false;
    //     while(i < size){
    //         if((i + 1) == size && !s) return false;
    //         switch(a[i] == b[i]){
    //             case true: {
    //                 s = true;
    //                 break;
    //             }
    //             case false: {
    //                 s = false;
    //                 break;
    //             }
    //         }
    //         i++;
    //     }
    // }
}