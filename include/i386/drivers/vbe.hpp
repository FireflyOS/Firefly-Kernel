#pragma once

#include <i386/multiboot2.hpp>

namespace firefly::drivers::vbe {
void scroll();
void putc(char c);
void puts(const char* str);
void putc(char c, int x, int y);
void put_pixel(int x, int y, int color);
void early_init(multiboot_tag_framebuffer* grub_fb);
}  // namespace firefly::drivers::vbe
