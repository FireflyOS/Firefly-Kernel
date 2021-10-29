#include "x86_64/fb/framebuffer.hpp"

#include <stl/cstdlib/cstring.h>
#include <stl/cstdlib/stdio.h>

#include "font8x16.h"
#include "x86_64/drivers/vbe.hpp"
#include "x86_64/fb/double-buffering.hpp"
#include "x86_64/libk++/bitmap.h"
#include <splash.h>

namespace firefly::kernel::device::fb {
static int console_x, console_y = 0;
static int glyph_width, glyph_height;
static uint8_t VBE_FONT[4096];
static int console_color = 0xFFFFFFFF;
static bool check_special(char c);

void init() {
    set_font(font, sizeof(font) / sizeof(font[0]), char_width, char_height, 0xFFFFFFF);
}

void set_font(uint8_t* fnt, int size, int fnt_w, int fnt_h, int color) {
    if (size > 4096) {
        //Font size not supported...
        return;
    }

    glyph_width = fnt_w;
    glyph_height = fnt_h;
    for (int i = 0; i < size; i++) {
        VBE_FONT[i] = fnt[i];
    }
    console_color = color;
}

void putc(char c, int x, int y) {
    check_special(c);
    for (int height = 0; height < glyph_height; height++) {
        for (int width = 0; width < glyph_width; width++) {
            if (VBE_FONT[(c * glyph_height) + height] bitand (1 << width)) {
                drivers::vbe::put_pixel(x + glyph_width - width, y + height, 0xFFFFFF);
            }
        }
    }
    console_x += glyph_width;
}

void putc(char c, int x, int y, int color) {
    check_special(c);
    for (int height = 0; height < glyph_height; height++) {
        for (int width = 0; width < glyph_width; width++) {
            if (VBE_FONT[(c * glyph_height) + height] bitand (1 << width)) {
                drivers::vbe::put_pixel(x + glyph_width - width, y + height, color);
            }
        }
    }
    console_x += glyph_width;
}

void putc(char c) {
    putc(c, console_x, console_y, console_color);
}

static bool check_special(char c) {
    libkern::Vec2 dim = drivers::vbe::fb_dimensions();
    //Todo: Check if c will go out of bounds
    if (c == '\n') {
        if (static_cast<size_t>(console_y) >= dim.y - glyph_height) {
            //scroll();
            console_y = dim.y - (glyph_height * 2);
            console_x = -glyph_width;
        }
        console_y += glyph_height;
        console_x = -glyph_width;
        return true;
    } else if (c == '\t') {
        console_x += 4;
        return true;
    } else if (c == '\b') {
        console_x -= 3;
        return true;
    }
    //Not a special char but it needs to be handled anyway so might as well put it in this function..
    else if (static_cast<size_t>(console_x) > dim.x - glyph_width) {
        console_x = -char_width;
        console_y++;
    }
    return false;
}

void puts(const char* str) {
    int i = 0;
    while (str[i] != '\0') {
        putc(str[i++], console_x, console_y, console_color);
    }
}

void boot_splash() {
    // Center image
    int framebuffer_width = drivers::vbe::fb_dimensions().x;
    int framebuffer_height = drivers::vbe::fb_dimensions().y;
    int x = (framebuffer_width  / 2) - (FIREFLY_WIDTH  / 2);
    int y = (framebuffer_height / 2) - (FIREFLY_HEIGHT / 2);

    int j = 0;
    for (uint32_t height = 0; height < FIREFLY_HEIGHT; height++) {
        for (uint32_t width = 0; width < FIREFLY_WIDTH; width++) {
            uint32_t pixel = (FIREFLY_DATA[j] << 16) | (FIREFLY_DATA[j + 1] << 8) | (FIREFLY_DATA[j + 2]);
            drivers::vbe::put_pixel(
                x + width,
                y + height,
                pixel
            );
            j+=3;
        }
    }
}

}  // namespace firefly::kernel::device::fb