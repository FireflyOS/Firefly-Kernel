#pragma once

struct sbif_main { //shell bitmap image format
    int x; //relative
    int y; //relative
    int gbar_color;
    bool eopl; //end of pixel line
};
struct sbif { //shell bitmap image format
    ///header
    int size; //size of image in pixels (x * y) without header

    sbif_main *sbif; //image data
};

namespace firefly::applications::shell::etc {
    extern sbif sbiftemp;
}