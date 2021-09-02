#include "x86_64/tty/double-buffering.hpp"

#include <stl/cstdlib/cstring.h>
#include <stl/cstdlib/stdio.h>

#include "font8x16.h"

namespace firefly::kernel::tty {
static tty_render_buffer front, back;


void DoubleBuffering::init_buffers() {
    front.is_front = true;
    back.is_front = false;

#if DEBUG
    front.id = "front";
    back.id = "back";
#endif

    //Active buffer: front, rendering to: back
    clear_buffer(0x0);

    //After swap_buffers:
    //Active buffer: back, rendering to: front
    // swap_buffers();
}

/* Render the contents of a buffer to the screen. */
void DoubleBuffering::render_buffer(tty_render_buffer& buffer) {
    libkern::Vec2 dimensions = firefly::drivers::vbe::fb_dimensions();
    for (size_t x = 0; x < dimensions.x; x++) {
        for (size_t y = 0; y < dimensions.y; y++) {
            drivers::vbe::put_pixel(x, y, buffer.buffer[y]);
        }
    }
    // memcpy(drivers::vbe::fb_addr(), buffer.buffer, dimensions.x * dimensions.y * drivers::vbe::fb_bpp());
}

void DoubleBuffering::fill_buffer(tty_render_buffer buffer, uint32_t color) {
    libkern::Vec2 dimensions = firefly::drivers::vbe::fb_dimensions();
    for (size_t x = 0; x < dimensions.x; x++) {
        for (size_t y = 0; y < dimensions.y; y++) {
            buffer.buffer[x] = color;
        }
    }
}

/* Write to a null terminated string to the back buffer */
void DoubleBuffering::write(const char* str,
                            int glyph_height, int glyph_width, int x, int y) {
    for (int i = 0, n = strlen(str); i < n; i++) {
        (void)x;
        (void)y;
        (void)glyph_height;
        (void)glyph_width;
        // this->write_char(str[i], glyph_height, glyph_width, x, y);
    }

    get_active().buffer[30] = 0xFFFFFF;

    swap_buffers();
}

/* Write a signled character to the back buffer (This does >NOT< swap buffers) */
void DoubleBuffering::write_char(char c, int glyph_height,
                                 int glyph_width, int x, int y) {
    for (int height = 0; height < glyph_height; height++) {
        for (int width = 0; width < glyph_width; width++) {
            if (font[(c * glyph_height) + height] bitand (1 << width)) {
                this->buffer_pixel(x + glyph_width - width, y + height, 0xFFFFFF);
            }
        }
    }
}

/* Plot a pixel in the back buffer */
void DoubleBuffering::buffer_pixel(size_t x, size_t y, uint32_t color) {
    for (size_t _y = 0; _y < drivers::vbe::fb_dimensions().y; _y++) {
        if (_y == y) {
            for (size_t _x = 0; _x < drivers::vbe::fb_dimensions().x; _x++) {
                if (_x == x) {
                    get_active().buffer[_y] = color;
                }
            }
        }
    }
}

void DoubleBuffering::clear_buffer(uint32_t color) {
    // printf("Using color %X on buffer %s\n", color, get_active().id);
    fill_buffer(get_active(), color);
    get_active().pos = get_active().pos.Empty();
}

/* Swap both buffers and render the previous back buffer to the screen */
void DoubleBuffering::swap_buffers() {
    front.is_front = !front.is_front;
    back.is_front = !back.is_front;
    back.is_front ? render_buffer(front) : render_buffer(back);
}

/* Return the active back buffer to draw to it */
tty_render_buffer
DoubleBuffering::get_active() {
    if (front.is_front)
        return back;
    return front;
}
}  // namespace firefly::kernel::tty