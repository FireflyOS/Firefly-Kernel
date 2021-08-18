#include <font8x16.h>
#include <stddef.h>
#include <stdint.h>
#include <stl/cstdlib/cstring.h>
#include <stl/cstdlib/stdio.h>

#include <x86_64/drivers/serial.hpp>
#include <x86_64/drivers/vbe.hpp>

namespace firefly::drivers::vbe {
static uint32_t* framebuffer_addr;
static size_t framebuffer_pitch;
static size_t framebuffer_height;
static size_t framebuffer_width;
// static size_t framebuffer_size;
static int console_x, console_y = 0;
static int glyph_width, glyph_height;
static uint8_t VBE_FONT[4096];

static bool check_special(char c);

void set_font(uint8_t* fnt, int size, int fnt_w, int fnt_h) {
    if (size > 4096) {
        //Font size not supported...
        return;
    }

    glyph_width = fnt_w;
    glyph_height = fnt_h;
    for (int i = 0; i < size; i++) {
        VBE_FONT[i] = fnt[i];
    }
}

void putc(char c, int x, int y) {
    check_special(c);
    for (int height = 0; height < glyph_height; height++) {
        for (int width = 0; width < glyph_width; width++) {
            if (VBE_FONT[(c * glyph_height) + height] bitand (1 << width)) {
                put_pixel(x + glyph_width - width, y + height, 0xFFFFFF);
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
                put_pixel(x + glyph_width - width, y + height, color);
            }
        }
    }
    console_x += glyph_width;
}

void putc(char c)
{
    putc(c, console_x, console_y);
}

static bool check_special(char c) {
    //Todo: Check if c will go out of bounds
    if (c == '\n') {
        if (static_cast<size_t>(console_y) > framebuffer_height) {
            scroll();
        }
        console_y += glyph_height;
        console_x = 0;
        return true;
    } else if (c == '\t') {
        console_x += 4;
        return true;
    }
    //Not a special char but it needs to be handled anyway so might as well put it in this function..
    else if (static_cast<size_t>(console_x) > framebuffer_width) {
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
        putc(str[i++], console_x, console_y);
    }
}

void scroll() {
    memcpy((void*)framebuffer_addr,
           (void*)((size_t)framebuffer_addr + framebuffer_width * glyph_height * 4),
           framebuffer_width * (framebuffer_height - glyph_height) * 4);
}

void put_pixel(int x, int y, int color) {
    framebuffer_addr[y * (framebuffer_pitch / sizeof(uint32_t)) + x] = color;
}

void early_init(stivale2_struct_tag_framebuffer* tagfb) {
    framebuffer_addr = reinterpret_cast<uint32_t*>(tagfb->framebuffer_addr);
    framebuffer_pitch = tagfb->framebuffer_pitch;
    framebuffer_height = tagfb->framebuffer_height;
    framebuffer_width = tagfb->framebuffer_width;
    // framebuffer_size = tagfb->common.size;

    set_font(font, sizeof(font) / sizeof(font[0]), char_width, char_height);
}
}  // namespace firefly::drivers::vbe
