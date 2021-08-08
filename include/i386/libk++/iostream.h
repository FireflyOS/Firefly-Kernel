#pragma once

#include <stdarg.h>

#include <i386/drivers/vga.hpp>
#include <i386/kernel.hpp>

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

#define klog(...) ({                                                         \
    ((firefly::libkern::get_cursor_handle() << __FUNCTION__                  \
                                            << ": " << __VA_ARGS__ << ' ')); \
})

namespace firefly::libkern {
using namespace firefly::drivers::vga;

firefly::drivers::vga::cursor& get_cursor_handle();
void globalize_vga_writer(firefly::drivers::vga::cursor& crs);

static auto cout = get_cursor_handle();
const auto endl = '\n';

template <typename... Tys>
void print(Tys... args) {
    ((firefly::libkern::get_cursor_handle() << args), ...);
}

}  // namespace firefly::libkern