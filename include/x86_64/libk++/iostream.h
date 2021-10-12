#pragma once

#include <stdarg.h>
#include <stl/cstdlib/stdio.h>

/*
 * (Listed in the most useful order)
 *
 * 3 ways of printing to the screen:
 *  1. klog(): Prints the function name it was called in.
 *     |_> Usage: `klog("Num: " << 123);`
 * 
 *  2. print(): Prints the arguments, seperated by commas ","
 *     |_> Usage: `firefly::libkern::print("Num: ", 123);`
 * 
 *  (Don't use it, it's only listed for the sake of completeness)
 *  3. firefly::libkern::cout = firefly::libkern::get_cursor_handle(); (Only do this once for each file)
 *     firefly::libkern::cout << "Num: " << 123 << firefly::libkern::endl;
 *   or
 *     firefly::libkern::get_cursor_handle() << "Num: " << 123 << firefly::libkern::endl;
*/

#define klog(...) ({                           \
    printf("%s:%d: ", __FUNCTION__, __LINE__); \
    printf(__VA_ARGS__);                       \
})
