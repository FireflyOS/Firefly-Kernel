#pragma once

#include <stddef.h>
#include <stdint.h>

#include "x86_64/drivers/vbe.hpp"
#include "x86_64/fb/framebuffer.hpp"

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
    void render_buffer(tty_render_buffer& buffer);
    tty_render_buffer& get_active();

private:
    tty_render_buffer front, back;

public:
    void buffer_pixel(size_t x, size_t y, uint32_t color);
    void swap_buffers();
    void init_buffers();
};
}  // namespace firefly::kernel::tty
