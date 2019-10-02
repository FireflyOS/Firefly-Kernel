#pragma once
#include "../algorithm.h"
#include "../../cstdlib/cstdint.h"
#include "../../cstdlib/cstring.h"

constexpr uint64_t VGA_BUFFER_ADDR = 0xb8000;
constexpr uint64_t WIDTH = 80;
constexpr uint64_t HEIGHT = 25;

enum class color : uint8_t {
    black = 0x00,
    blue = 0x01,
    green = 0x02,
    cyan = 0x03,
    red = 0x04,
    magenta = 0x05,
    brown = 0x06,
    lightgrey = 0x07,
    darkgrey = 0x08,
    lightblue = 0x09,
    lightgreen = 0x0A,
    lightcyan = 0x0B,
    lightred = 0x0C,
    lightmagenta = 0x0D,
    lightbrown = 0x0E,
    white = 0x0F,
};

constexpr static size_t width = 80;
constexpr static size_t height = 15;
constexpr static size_t display_buff_addr = 0xB8000;

struct Display {

    size_t y = 0;
    size_t x = 0;

    short background = static_cast<short>(color::black);
    short foreground = static_cast<short>(color::white);

    char16_t* display_buffer = reinterpret_cast<char16_t*>(display_buff_addr);

    char16_t prepare_char(const char16_t c) const noexcept {
        return c | (((background << 4) | (foreground & 0x0F)) << 8);
    }

    void clear() noexcept {
        // firefly::std::fill(
        //     display_buffer, display_buffer + (height * width), prepare_char(' ')
        // );
    }

    void handle_write_pos() noexcept {
        if (x >= width) {
            y++;
            x = 0;
        }
        if (y >= height) {
            firefly::std::copy(display_buffer + width, display_buffer + ((height - 1) * width), display_buffer);
            y = height - 1;
        }
    }

    bool handle_special_characters(const char16_t c) noexcept {
        bool ret = false;
        if (c == '\n') {
            x = 0;
            y++;
            ret = true;
        }
        if (c == '\t') {
            x += 4;
            ret = true;
        }
        if (c == '\r') {
            x = 0;
            ret = true;
        }
        if (ret) handle_write_pos();
        return ret;
    }

    void write(const uint16_t c) noexcept {
        if (handle_special_characters(c)) {
            return;
        }
        display_buffer[y * width + x++] = prepare_char(c);
        handle_write_pos();
    }

    void write(const uint16_t* str) noexcept {
        for (size_t idx = 0; idx < strnlen(str); idx++) {
            write(str[idx]);
        }
    }

    void write(const char c) noexcept {
        if (handle_special_characters(c)) {
            return;
        }
        display_buffer[y * width + x++] = prepare_char(c);
        handle_write_pos();
    }

    void write(const char* str) noexcept {
        for (size_t idx = 0; idx < strnlen(str); idx++) {
            write(str[idx]);
        }
    }

    void set_background_color(color c) noexcept {
        background = static_cast<short>(c);
    }

    void set_foreground_color(color c) noexcept {
        foreground = static_cast<short>(c);
    }

};