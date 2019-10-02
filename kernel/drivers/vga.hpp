#pragma once
#include <cppstdlib/algorithm.h>
#include <cstdint.h>

constexpr size_t VGA_BUFFER_ADDR = 0xB8000;
constexpr size_t WIDTH = 80;
constexpr size_t HEIGHT = 25;

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

class VgaBuffer {
private:
    size_t x = 0;
    size_t y = 0;

    uint8_t bg_color = static_cast<int>(color::black);
    uint8_t text_color = static_cast<int>(color::white);

    uint16_t* arr = reinterpret_cast<uint16_t*>(VGA_BUFFER_ADDR);

    uint16_t prepare_char(uint16_t chr) {
        return ((static_cast<uint16_t>(bg_color) | static_cast<uint16_t>(text_color)) << 8) | chr;
    }

    void set_bg_color(color c) {
        this->bg_color = static_cast<int>(c);
    }

    void set_text_color(color c) {
        this->text_color = static_cast<int>(c)
    }

public:
    VgaBuffer() {
        this->clear();
    }

    void clear() {
        auto prepared_char = this->prepare_char(' ');
        for (auto* _ptr = arr; _ptr < (arr + (WIDTH * HEIGHT); _ptr = prepared_char))
            ;
    }


    void write(uint16_t chr) {
        arr[(y * HEIGHT) + x] = prepare_char(chr);
    }

    void write(const char* chr) {
    }
};