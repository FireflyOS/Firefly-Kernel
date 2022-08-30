#include "firefly/console/console.hpp"

#include <cstdlib/cstring.h>
#include <stddef.h>

#include "limine-terminal-port/term.h"

extern uintptr_t _binary_fonts_vgafont_bin_start[];

namespace firefly::kernel::console {

constinit term_t term = {};

void init(stivale2_struct_tag_framebuffer *fb) {
	(void)fb;
    // struct framebuffer_t frm = {
    //     fb->framebuffer_addr,
    //     fb->framebuffer_width,
    //     fb->framebuffer_height,
    //     fb->framebuffer_pitch
    // };

    // auto font_addr = (uintptr_t)_binary_fonts_vgafont_bin_start;
    // struct font_t font = {
    //     font_addr,  // Address of font file
    //     8,          // Font width
    //     16,         // Font height
    //     1,          // Character spacing
    //     0,          // Font scaling x
    //     0           // Font scaling y
    // };

    // struct style_t style = {
    //     DEFAULT_ANSI_COLOURS,         // Default terminal palette
    //     DEFAULT_ANSI_BRIGHT_COLOURS,  // Default terminal bright palette
    //     0xA0000000,                   // Background colour
    //     0xFFFFFF,                     // Foreground colour
    //     0,                            // Terminal margin
    //     0                             // Terminal margin gradient
    // };

    // struct background_t back {};

    // term_init(&term, nullptr, false);  // if set to true memory is allocated for a vga terminal - we don't need that.
    // term_vbe(&term, frm, font, style, back);
}

// Todo: Overload this function with a string view
void write(const char *str) {
	(void)str;
    // term_print(&term, str);
}
}  // namespace firefly::kernel::console