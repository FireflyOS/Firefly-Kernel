#include <font8x16.h>
#include <splash.h>
#include <stddef.h>
#include <stdint.h>
#include <stl/cstdlib/cstring.h>
#include <stl/cstdlib/stdio.h>

#include <x86_64/drivers/serial.hpp>
#include <x86_64/drivers/vbe.hpp>


namespace firefly::drivers::vbe {
static uint32_t* framebuffer_addr;
static size_t framebuffer_height;
static size_t framebuffer_width;
static size_t framebuffer_pitch;

uint32_t* fb_addr() { return framebuffer_addr; }
libkern::Vec2 fb_dimensions()
{
    return { framebuffer_width, framebuffer_height };
}

void clear_splash_frame();

void put_pixel(int x, int y, int color) {
    framebuffer_addr[y * (framebuffer_pitch / sizeof(uint32_t)) + x] = color;
}

void early_init(stivale2_struct_tag_framebuffer* tagfb) {
    framebuffer_addr = reinterpret_cast<uint32_t*>(tagfb->framebuffer_addr);
    framebuffer_pitch = tagfb->framebuffer_pitch;
    framebuffer_height = tagfb->framebuffer_height;
    framebuffer_width = tagfb->framebuffer_width;
}

void clear_splash_frame() {
    int x = framebuffer_width / 3 + (splash_width / 3);
    int y = framebuffer_height / 3;

    for (int height = 0; height < splash_height; height++) {
        for (int width = 0; width < splash_width; width++) {
            uint32_t pixel = 0x0;
            put_pixel(
                x + width,
                y + height,
                pixel);
        }
    }
}

void boot_splash() {
    int x = framebuffer_width / 3 + (splash_width / 3);
    int y = framebuffer_height / 3;

    int j = 0;
    for (int height = 0; height < splash_height; height++) {
        for (int width = 0; width < splash_width; width++) {
            j++;
            uint32_t pixel = (splash_screen_pixel_data[j] << 24 & 0xFF000000) | (splash_screen_pixel_data[j] << 16 & 0x00FF0000) | (splash_screen_pixel_data[j] << 8 & 0x0000FF00) | (splash_screen_pixel_data[j] & 0x000000FF);
            put_pixel(
                x + width,
                y + height,
                pixel);
        }
    }
}

}  // namespace firefly::drivers::vbe
