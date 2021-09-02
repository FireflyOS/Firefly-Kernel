#pragma once

#include <stddef.h>
#include <stdint.h>

#include "x86_64/drivers/vbe.hpp"
#include "x86_64/tty/boot-tty.hpp"

namespace firefly::kernel::tty {
typedef struct generic_display_buffer {
    uint32_t* buffer;
    libkern::Vec2 pos;
#if DEBUG
    const char* id;  //Determine the type of buffer used
#endif
    bool is_front;  //Is this buffer being used as the front buffer?
} tty_render_buffer;

class DoubleBuffering {
private:
    void write_char(char c, int glyph_height, int glyph_width, int x, int y);
    void fill_buffer(tty_render_buffer buffer, uint32_t color);
    void buffer_pixel(size_t x, size_t y, uint32_t color);
    void render_buffer(tty_render_buffer& buffer);
    void clear_buffer(uint32_t color);
    tty_render_buffer get_active();

public:
    void write(const char* str, int glyph_height, int glyph_width, int x, int y);
    void swap_buffers();
    void init_buffers();
};
}  // namespace firefly::kernel::tty
