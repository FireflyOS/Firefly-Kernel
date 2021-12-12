#pragma once

namespace firefly::mm {
    inline void *malloc(int size){
        char a[size];
        return &a;
    }
}