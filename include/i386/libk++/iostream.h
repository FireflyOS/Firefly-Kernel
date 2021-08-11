#pragma once

#include <stdarg.h>

#include <i386/drivers/vga.hpp>
#include <i386/kernel.hpp>

/*
 *
 *  Ways of printing to the screen:
 *  1. klog(): Prints the function name and the line number it was called in.
 *     |_> Usage: `klog() << "Num: " << 123;`
 * 
 *  2. print(): Prints the arguments, seperated by commas ","
 *     |_> Usage: `firefly::libkern::print("Num: ", 123);`
 * 
 *  3. printf(): Printf like formatting (Can be found in stdio.cpp)
*/

#define klog() firefly::kernel::main::Singleton::Get().internal_cursor_handle() << __FUNCTION__ << ":" << static_cast<size_t>(__LINE__) << ": "

namespace firefly::libkern {
using namespace firefly::drivers::vga;

const auto endl = '\n';

template <typename... Tys>
void print(Tys... args) {
    ((firefly::kernel::main::Singleton::Get().internal_cursor_handle() << args), ...);
}

}  // namespace firefly::libkern