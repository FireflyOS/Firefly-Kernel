#include <drivers/vga.hpp>
#include "drivers/ports.hpp"

vga_char cursor::character(char c) {
    return { c, fg, bg };
}

Display& Display::operator<<(const char* arr) {
    this->write(arr);
    return *this;
}

Display& Display::operator<<(char c) {
    this->write(c);
    return *this;
}

void Display::clear() {
    firefly::std::fill(
        display_buffer, display_buffer + (height * width), vga_char{ ' ', color::black, color::black });
}

[[nodiscard]] size_t Display::vgalen(const vga_char* str) {
    size_t _sz{};
    while (str[_sz++].codepoint)
        ;
    return _sz;
}

void Display::handle_write_pos() {
    if (crs.x >= width) {
        crs.y++;
        crs.x = 0;
    }
    if (crs.y >= height) {
        firefly::std::copy(display_buffer + width, display_buffer + ((height - 1) * width), display_buffer);
        crs.y = height - 1;
    }
    update_cursor(crs.x, crs.y);
}

bool Display::handle_special_characters(const char c) {
    bool ret = false;
    if (c == '\n') {
        crs.x = 0;
        crs.y++;
        ret = true;
    } else if (c == '\t') {
        crs.x += 4;
        ret = true;
    } else if (c == '\r') {
        crs.x = 0;
        ret = true;
    } else if (c == '\b') {
        /** bugged 
         * 
         * 
         * When character is first character of a line, it's buggy
        */
        if (crs.y == 0 && crs.x == 0) {
        } else {
            display_buffer[crs.y * width + crs.x - 1] = { ' ', color::white, color::black };
            display_buffer[crs.y * width + crs.x] = { ' ', color::white, color::black };
            if (crs.x-- == 0) {
                crs.y--;
                crs.x = width - 1;
            }
        }
        return true;
    } else if (c == '\0') {
        return true;
    }
    if (ret) handle_write_pos();
    return ret;
}

void Display::write(const vga_char c) {
    if (handle_special_characters(c.codepoint)) {
        update_cursor(crs.x, crs.y);
        return;
    }
    display_buffer[crs.y * width + crs.x++] = c;
    handle_write_pos();
    update_cursor(crs.x, crs.y);
}

void Display::write(const vga_char* str) {
    for (size_t idx = 0; idx < vgalen(str); idx++) {
        write(str[idx]);
    }
}

void Display::write(const char c) {
    if (handle_special_characters(c)) {
        return;
    }
    display_buffer[crs.y * width + crs.x++] = crs.character(c);
    handle_write_pos();
}

void Display::write(const char* str) {
    for (size_t idx = 0; idx < strlen(str); idx++) {
        write(str[idx]);
    }
}

void Display::set_background_color(color c) {
    crs.bg = c;
}

void Display::set_foreground_color(color c) {
    crs.fg = c;
}

void Display::toggle_cursor(bool on) {
    _visual_cursor = on;
    if (on) {
    } else {
    }
}

void Display::update_cursor(size_t x, size_t y) {
    (void)x;
    (void)y;
}