#include "x86_64/fb/double-buffering.hpp"

#include <stl/cstdlib/cstring.h>
#include <stl/cstdlib/stdio.h>

#include "font8x16.h"

namespace firefly::kernel::tty {

void DoubleBuffering::init_buffers() {
    //Todo: Alloc buffers once we have the mm bug fixed
    //alloc(this->front)
    //alloc(this->back);

    front.is_front = true;
    back.is_front = false;

#if DEBUG
    front.id = "front";
    back.id = "back";
#endif
}

/* Render the contents of a buffer to the screen. */
void DoubleBuffering::render_buffer(tty_render_buffer& buffer) {
    libkern::Vec2 dimensions = firefly::drivers::vbe::fb_dimensions();
    memcpy(
        (void*)drivers::vbe::fb_addr(),
        buffer.buffer,
        dimensions.y * drivers::vbe::fb_pitch()
    );
}

void DoubleBuffering::buffer_pixel(size_t x, size_t y, uint32_t color) {
    get_active().buffer[(y * (drivers::vbe::fb_pitch() / sizeof(uint32_t))) + x] = color;
}

/* Swap both buffers and render the previous back buffer to the screen */
void DoubleBuffering::swap_buffers() {
    front.is_front = !front.is_front;
    back.is_front = !back.is_front;
    back.is_front ? render_buffer(front) : render_buffer(back);
}

/* Return the active back buffer to draw to it */
tty_render_buffer& DoubleBuffering::get_active() {
    if (front.is_front)
        return back;
    return front;
}
}  // namespace firefly::kernel::tty
