#include "utils.hpp"

void start_load(Display& disp, const char* _str) {
    disp << "[";
    disp.set_foreground_color(color::blue);
    disp << "START";
    disp.set_foreground_color(color::white);
    disp << "]" << _str << "\n";
}

void end_load(Display& disp, const char* _str) {
    disp << "[";
    disp.set_foreground_color(color::green);
    disp << "OK";
    disp.set_foreground_color(color::white);
    disp << "]" << _str << "\n";
}