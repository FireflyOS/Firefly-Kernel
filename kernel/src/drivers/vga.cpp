#include <drivers/vga.hpp>
#include "drivers/ports.hpp"
#include "utils.hpp"

vga_char cursor::character(char c) {
    return { c, fg, bg };
}

cursor& cursor::operator<<(const char* c) {
    vga::write(c, *this);
    return *this;
}

cursor& cursor::operator<<(char c) {
    vga::write(c, *this);
    return *this;
}

void cursor::set_foreground_color(color c) {
    fg = c;
}

void cursor::set_background_color(color c) {
    bg = c;
}

void cursor::handle_bounds() {
    // right now assume only one cursor owns the entire screen, handle bounds later
    if (x >= width) {
        y++;
        x = 0;
    }
    if (y >= height) {
        firefly::std::copy(display_buffer + width, display_buffer + (height * width), display_buffer);
        firefly::std::fill(display_buffer + ((height - 1) * width), display_buffer + (height * width), vga_char{ ' ', color::white, color::black });
        y = height - 1;
    }
    vga::update_hw_cursor(x, y);
}

bool vga::init() {
    // move CRT controller registers to 0x3Dn
    uint8_t msr = firefly::inb(0x3CC);
    firefly::outb(0x3C2, msr & 0x01);
    enable_hw_cursor();
    return true;
}

void vga::clear() {
    firefly::std::fill(
        display_buffer, display_buffer + 9, vga_char{ ' ', color::white, color::black });
}

[[nodiscard]] size_t vgalen(const vga_char* str) {
    size_t _sz{};
    while (str[_sz++].codepoint)
        ;
    return _sz;
}

// void vga::handle_write_pos() {
//     if (crs.x >= width) {
//         crs.y++;
//         crs.x = 0;
//     }
//     if (crs.y >= height) {
//         firefly::std::copy(display_buffer + width, display_buffer + ((height - 1) * width), display_buffer);
//         crs.y = height - 1;
//     }
//     update_hw_cursor(crs.x, crs.y);
// }

bool vga::handle_special_characters(const char c, cursor& crs) {
    bool ret = false;

    switch (c) {
        case '\n':
            crs.x = 0;
            crs.y++;
            ret = true;
            break;
        case '\t':
            crs.x += 4;
            ret = true;
            break;
        case '\r':
            crs.x = 0;
            ret = true;
            break;
        case '\0':
            return true;
        case '\b':
            if (!(crs.y == 0 && crs.x == 0)) {
                display_buffer[crs.y * width + crs.x - 1] = { ' ', color::white, color::black };
                // display_buffer[crs.y * width + crs.x] = { ' ', color::white, color::black };
                if (crs.x-- == 0) {
                    crs.y--;
                    crs.x = width - 1;
                }
            }
            return true;
    }

    if (ret)
        crs.handle_bounds();
    return ret;
}

void vga::write(const vga_char c, cursor& crs, bool _update) {
    if (handle_special_characters(c.codepoint, crs)) {
        if (_update)
            update_hw_cursor(crs.x, crs.y);
        return;
    }
    display_buffer[crs.y * width + crs.x++] = c;
    crs.handle_bounds();
    if (_update)
        update_hw_cursor(crs.x, crs.y);
}

void vga::write(const vga_char* str, cursor& crs) {
    for (size_t idx = 0; idx < vgalen(str); idx++)
        write(str[idx], crs, false);
    update_hw_cursor(crs.x, crs.y);
}

void vga::write(const char c, cursor& crs, bool _update) {
    if (handle_special_characters(c, crs)) {
        if (_update)
            update_hw_cursor(crs.x, crs.y);
        return;
    }

    display_buffer[crs.y * width + crs.x++] = crs.character(c);
    crs.handle_bounds();
    if (_update)
        update_hw_cursor(crs.x, crs.y);
}

void vga::write(const char* str, cursor& crs) {
    for (size_t idx = 0; idx < strlen(str); idx++)
        write(str[idx], crs, false);
    update_hw_cursor(crs.x, crs.y);
}

void vga::enable_hw_cursor() {
    _visual_cursor = true;

    // draw cursor on lowest scan line
    firefly::outb(0x3D4, 0x0A);
    firefly::outb(0x3D5, 15);
    firefly::outb(0x3D4, 0x0B);
    firefly::outb(0x3D5, 15 | 0x60);
}

void vga::disable_hw_cursor() {
    _visual_cursor = false;

    // set disable bit
    firefly::outb(0x3D4, 0x0A);
    firefly::outb(0x3D5, 0x20);
}

void vga::update_hw_cursor(size_t x, size_t y) {
    uint16_t pos = y * width + x;

    firefly::outb(0x3D4, 0x0F);
    firefly::outb(0x3D5, static_cast<uint8_t>(pos & 0xFF));
    firefly::outb(0x3D4, 0x0E);
    firefly::outb(0x3D5, static_cast<uint8_t>((pos >> 8) & 0xFF));
}