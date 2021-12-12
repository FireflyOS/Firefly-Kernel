#pragma once

#include <x86_64/stivale2.hpp>

namespace firefly::drivers::vbe {
void scroll();
void putc(char c);
void putc(char c, int x, int y);
void putc(char c, int x, int y, int color);
void puts(const char* str);
void put_pixel(int x, int y, int color);
int get_pixel(int x, int y);
void early_init(stivale2_struct_tag_framebuffer* tagfb);
void set_shell();
void boot_splash();
}  // namespace firefly::drivers::vbe
