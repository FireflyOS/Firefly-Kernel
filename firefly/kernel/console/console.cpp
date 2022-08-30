#include "firefly/console/console.hpp"

#include <cstdlib/cstring.h>
#include <stddef.h>

#include "firefly/compiler/clang++.hpp"
#include "firefly/limine.hpp"
#include "limine-terminal-port/term.h"

extern uintptr_t _binary_fonts_vgafont_bin_start[];

namespace firefly::kernel::console {

USED struct limine_framebuffer_request fb {
    .id = LIMINE_FRAMEBUFFER_REQUEST, .revision = 0, .response = nullptr
};

constinit term_t term = {};

void init() {
    auto fb_response = fb.response;
    if (unlikely(!fb_response))
        return;

    struct framebuffer_t frm = {
        (uintptr_t)fb_response->framebuffers[0]->address,  // Framebuffer address
        fb_response->framebuffers[0]->width,               // Framebuffer width
        fb_response->framebuffers[0]->height,              // Framebuffer height
        fb_response->framebuffers[0]->pitch                // Framebuffer pitch
    };

    auto font_addr = (uintptr_t)_binary_fonts_vgafont_bin_start;
    struct font_t font = {
        font_addr,  // Address of font file
        8,          // Font width
        16,         // Font height
        1,          // Character spacing
        0,          // Font scaling x
        0           // Font scaling y
    };

    struct style_t style = {
        DEFAULT_ANSI_COLOURS,         // Default terminal palette
        DEFAULT_ANSI_BRIGHT_COLOURS,  // Default terminal bright palette
        0xA0000000,                   // Background colour
        0xFFFFFF,                     // Foreground colour
        0,                            // Terminal margin
        0                             // Terminal margin gradient
    };

    struct background_t back {};

    term_init(&term, nullptr, false);  // if set to true memory is allocated for a vga terminal - we don't need that.
    term_vbe(&term, frm, font, style, back);
}

// Todo: Overload this function with a string view
void write(const char *str) {
    term_print(&term, str);
}
}  // namespace firefly::kernel::console