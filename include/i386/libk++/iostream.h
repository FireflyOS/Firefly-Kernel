#pragma once

#include <stdarg.h>
#include <stl/cstdlib/stdio.h>

#include <i386/drivers/vga.hpp>

/*
 *
 *  Ways of printing to the screen:
 *  1. klog(): Prints the function name and the line number it was called in.
 *     |_> Usage: `klog("Num: %d", 123);`
 * 
 *  3. printf(): Printf like formatting (Can be found in stdio.cpp)
*/

#define klog(...) ({                           \
    printf("%s:%d: ", __FUNCTION__, __LINE__); \
    printf(__VA_ARGS__);                       \
})