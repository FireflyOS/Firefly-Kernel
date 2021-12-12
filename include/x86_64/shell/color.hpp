#pragma once

namespace firefly::kernel::shell {
    inline int GBAR(unsigned char r, unsigned char g, unsigned char b, unsigned char a, unsigned char lightness){
        int res;

        ((char*)&res)[0] = r - lightness;
        ((char*)&res)[1] = a - lightness;
        ((char*)&res)[2] = b - lightness;
        ((char*)&res)[3] = g - lightness;

        return res;
    }
}