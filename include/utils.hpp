#pragma once
#include "drivers/vga.hpp"

// If you ever need to just build the kernel to see if there are any errors
// without getting pedantic errors on unused variables, use this.
#define IGNORE_PEDANTIC_ERROR(var) ({ [[maybe_unused]] decltype(var) var = var; })

void start_load(cursor& crs, const char* _str);
void end_load(cursor& crs, const char* _str);