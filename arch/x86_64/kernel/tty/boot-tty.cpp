#include "x86_64/tty/boot-tty.hpp"

#include <stl/cstdlib/cstring.h>

#include "font8x16.h"
#include "x86_64/drivers/vbe.hpp"

namespace firefly::kernel::tty {
static int console_x, console_y = 0;
static int glyph_width, glyph_height;
static uint8_t VBE_FONT[4096];
static int console_color = 0xFFFFFFFF;
static bool check_special(char c);

void init() {
    set_font(font, sizeof(font) / sizeof(font[0]), char_width, char_height, 0xFFFFFFF);
}

void scroll() {
    // clear_splash_frame();
    libkern::Vec2 dim = drivers::vbe::fb_dimensions();
    uint32_t* framebuffer_addr = drivers::vbe::fb_addr();
    memcpy((void*)framebuffer_addr, (void*)((size_t)framebuffer_addr + dim.x * glyph_height * (4)), dim.x * (dim.y - glyph_height) * 6);
    // boot_splash();
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
            scroll();  //TODO: Use double buffering impl
            console_y = dim.y - (glyph_height * 2);
            console_x = glyph_width;
        }
        console_y += glyph_height;
        console_x = 0;
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
        console_x = 0;
        console_y++;
    }
    return false;
}

void puts(const char* str) {
    int i = 0;
    while (str[i] != '\0') {
        if (check_special(str[i])) {
            i++;
            continue;
        }
        putc(str[i++], console_x, console_y, console_color);
    }
}
}  // namespace firefly::kernel::tty