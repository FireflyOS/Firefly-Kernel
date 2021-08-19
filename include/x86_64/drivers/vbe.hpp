#pragma once

#include <x86_64/stivale2.hpp>

namespace firefly::drivers::vbe {
void scroll();
void putc(char c);
void puts(const char* str);
void put_pixel(int x, int y, int color);
void early_init(stivale2_struct_tag_framebuffer* tagfb);
void boot_splash();
}  // namespace firefly::drivers::vbe
