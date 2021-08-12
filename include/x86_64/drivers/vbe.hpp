#pragma once

#include <x86_64/multiboot2.hpp>

namespace firefly::drivers::vbe {
void scroll();
void puts(const char* str);
void put_pixel(int x, int y, int color);
void early_init(multiboot_tag_framebuffer* grub_fb);
}  // namespace firefly::drivers::vbe
