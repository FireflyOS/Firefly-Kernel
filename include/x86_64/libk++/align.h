#pragma once

#include <stdint.h>

namespace firefly::libkern
{
    /*
        align<size_t>(var); - Align `var` on a 4096 byte boundary
    */
    template <typename T>
    inline void align4k(T& in)
    {
        in = (in + 4096 - 1) & ~(4096 - 1);
    }

    /*
        align<size_t, int32_t>(var); - Align `var` on a 32 byte boundary
    */
    template <typename T, typename S>
    inline void align(T& in, S size)
    {
        in = (in + sizeof(size) - 1) & ~(sizeof(size) - 1);
    }

    /*
        align<size_t>(var, 16); - Align `var` on a 16 byte boundary
    */
    template <typename T>
    inline void align(T& in, size_t size)
    {
        in = (in + size - 1) & ~(size - 1);
    }

}  // namespace firefly::libkern
