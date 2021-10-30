#include <stddef.h>
#include <stdint.h>
#include <stl/cstdlib/cstring.h>
#include <stl/cstdlib/stdio.h>

#include "font8x16.h"
#include "splash.h"
#include "x86_64/drivers/serial.hpp"
#include "x86_64/drivers/vbe.hpp"
#include "x86_64/memory-manager/relocation.hpp"

namespace firefly::drivers::vbe {
static uint32_t* framebuffer_addr;
static size_t framebuffer_height;
static size_t framebuffer_width;
static size_t framebuffer_pitch;
static size_t framebuffer_bpp;

uint32_t* fb_addr() { return framebuffer_addr; }
libkern::Vec2 fb_dimensions()
{
    return { framebuffer_width, framebuffer_height };
}
size_t fb_bpp() { return framebuffer_bpp; }
size_t fb_pitch() { return framebuffer_pitch; }
void clear_splash_frame();

void put_pixel(int x, int y, int color) {
    framebuffer_addr[y * (framebuffer_pitch / sizeof(uint32_t)) + x] = color;
}

void early_init(stivale2_struct_tag_framebuffer* tagfb) {
    framebuffer_addr = reinterpret_cast<uint32_t*>(
        mm::relocation::conversion::from_higher_half(
            tagfb->framebuffer_addr, mm::relocation::conversion::DATA
        )
    );
    framebuffer_pitch = tagfb->framebuffer_pitch;
    framebuffer_height = tagfb->framebuffer_height;
    framebuffer_width = tagfb->framebuffer_width;
    framebuffer_bpp = tagfb->framebuffer_bpp;
}

void clear_splash_frame() {
    // int x = framebuffer_width / 3 + (splash_width / 3);
    // int y = framebuffer_height / 3;

    // for (int height = 0; height < splash_height; height++) {
    //     for (int width = 0; width < splash_width; width++) {
    //         uint32_t pixel = 0x0;
    //         put_pixel(
    //             x + width,
    //             y + height,
    //             pixel);
    //     }
    // }
}

}  // namespace firefly::drivers::vbe
