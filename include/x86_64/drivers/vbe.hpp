#pragma once

namespace firefly::drivers::vbe {
void scroll();
void putc(char c);
void puts(const char* str);
void put_pixel(int x, int y, int color);
// void early_init(multiboot_tag_framebuffer* grub_fb);
}  // namespace firefly::drivers::vbe
