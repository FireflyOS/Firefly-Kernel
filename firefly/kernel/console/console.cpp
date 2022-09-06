#include "firefly/console/console.hpp"

#include <cstddef>

#include "firefly/compiler/compiler.hpp"
#include "firefly/console/term.h"
#include "firefly/graphics/framebuffer.hpp"
#include "firefly/intel64/acpi/acpi.hpp"
#include "firefly/limine.hpp"
#include "firefly/logger.hpp"
#include "firefly/memory-manager/virtual/virtual.hpp"
#include "libk++/align.h"

extern uintptr_t _binary_fonts_vgafont_bin_start[];

namespace firefly::kernel::console {

USED struct limine_framebuffer_request fb {
    .id = LIMINE_FRAMEBUFFER_REQUEST, .revision = 0, .response = nullptr
};

// TODO: Support multiple terminals.
// A simple frg::array should do the trick.
// We could even have different terminal background images.
// Implementation: A config file can be passed as a bootloader module and parsed here.
term_t term;

background_t loadTerminalSplash();

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

    // TODO: Extend the fbDev class to the C++ file, not just header only.
    //       It's best if fbDev initializes the console, there should also be a frg::vector (requires a slab allocator)
    //		 for every console / framebuffer / instance of fbDev since limine supports multiple framebuffers now
    fbDev.init(PhysicalAddress(frm.address), frm.width, frm.height, frm.pitch);

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
        0xAAAAAA,                     // Foreground colour
        0,                            // Terminal margin
        0                             // Terminal margin gradient
    };

    term_init(&term, nullptr);

    // Try to load the firmware bootsplash as the terminal background.
    // If we can't find it we default to black
    auto const bgrt = reinterpret_cast<AcpiBgrt *>(core::acpi::Acpi::accessor().find("BGRT"));

    if (bgrt && bgrt->valid()) {
        // Bitmap header
        struct PACKED {
            uint16_t id;
            uint32_t size;
        } hdr;
        memcpy(&hdr, (const void *)bgrt->imageAddress, sizeof(decltype(hdr)));

        struct image_t img;
        image_open(&img, bgrt->imageAddress, hdr.size);
        struct background_t back = {
            &img,
            CENTERED,
            0
        };
        term_vbe(&term, frm, font, style, back);
    } else {
        term_vbe(&term, frm, font, style, {});
    }
}

void write(const char *str) {
    if (likely(term.initialised))
        term_print(&term, str);
}

}  // namespace firefly::kernel::console