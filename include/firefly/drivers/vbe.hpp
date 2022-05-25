#pragma once

#include "firefly/stivale2.hpp"
#include "firefly/libk++/vector_math.h"

namespace firefly::drivers::vbe {
void scroll();
uint32_t* fb_addr();
libkern::Vec2 fb_dimensions();
size_t fb_bpp();
size_t fb_pitch();

void put_pixel(int x, int y, int color);
void early_init(stivale2_struct_tag_framebuffer* tagfb);
void boot_splash();
}  // namespace firefly::drivers::vbe
