#include <font8x16.h>
#include <splash.h>
#include <stddef.h>
#include <stdint.h>
#include <stl/cstdlib/cstring.h>
#include <stl/cstdlib/stdio.h>

#include <x86_64/drivers/serial.hpp>
#include <x86_64/drivers/serial_legacy.hpp>
#include <x86_64/drivers/vbe.hpp>


namespace firefly::drivers::vbe {
static uint32_t* framebuffer_addr;
static size_t framebuffer_pitch;
static size_t framebuffer_height;
static size_t framebuffer_width;
static int console_x, console_y = 0;
static int glyph_width, glyph_height;
static uint8_t VBE_FONT[4096];
static int console_color = 0xFFFFFFFF;

static bool check_special(char c);

static bool shell;

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

void putc(char c) {
    putc(c, console_x, console_y, console_color);
}

static bool check_special(char c) {
    if(console_x == (128 * glyph_width)) c = '\n';
    
    if (c == '\n') {
        if (static_cast<size_t>(console_y) >= framebuffer_height - glyph_height) {
            scroll();
            console_y = framebuffer_height - (glyph_height * 2);
            console_x = glyph_width;
        }
        console_y += glyph_height;
        console_x = 0;
        return true;
    } else if (c == '\t') {
        console_x += 4;
        return true;
    } else if (c == '\b') {
        console_x -= 8;
        return true;
    }
    //Not a special char but it needs to be handled anyway so might as well put it in this function..
    else if (static_cast<size_t>(console_x) > framebuffer_width - glyph_width) {
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

void clearx(int y_offset) {
    for (size_t x = 0; x < framebuffer_width; x++) {
        putc('|', x, y_offset, 0x0);
    }
}

void video_scroll_up(void* start, void* end, void* dest) {
    memcpy(start, dest, ((char*)end - (char*)start) << 2);
}

void clear_splash_frame();
void scroll() {
    clear_splash_frame();
    memcpy((void*)framebuffer_addr, (void *)((size_t)framebuffer_addr + framebuffer_width * glyph_height * (4)), framebuffer_width * (framebuffer_height - glyph_width) * 6);
    boot_splash();
}

//GBAR is used
void put_pixel(int x, int y, int color) {
    framebuffer_addr[y * (framebuffer_pitch / sizeof(uint32_t)) + x] = color;
}

//GBAR is used
int get_pixel(int x, int y){
    return framebuffer_addr[y * (framebuffer_pitch / sizeof(uint32_t)) + x];
}

void early_init(stivale2_struct_tag_framebuffer* tagfb) {
    framebuffer_addr = reinterpret_cast<uint32_t*>(tagfb->framebuffer_addr);
    framebuffer_pitch = tagfb->framebuffer_pitch;
    framebuffer_height = tagfb->framebuffer_height;
    framebuffer_width = tagfb->framebuffer_width;

    set_font(font, sizeof(font) / sizeof(font[0]), char_width, char_height, 0xFFFFFFF);
}

void clear_splash_frame() {
    if(!shell){
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
}

void set_shell(){
    shell = true;
}

void boot_splash() {
    if(!shell){ //temp
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
}

}  // namespace firefly::drivers::vbe
