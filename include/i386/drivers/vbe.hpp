#pragma once

namespace firefly::drivers::vbe {
void scroll();
void putc(char c);
void puts(const char* str);
void putc(char c, int x, int y);
void put_pixel(int x, int y, int color);
// void early_init(multiboot_tag_framebuffer* grub_fb);
size_t get_screen_dimension_x();
size_t get_screen_dimension_y();

void boot_splash();
}  // namespace firefly::drivers::vbe
