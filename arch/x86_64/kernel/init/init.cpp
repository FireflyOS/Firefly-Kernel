#include <stddef.h>
#include <stdint.h>
#include <x86_64/libk++/iostream.h>

#include <x86_64/drivers/vbe.hpp>
#include <x86_64/gdt/gdt.hpp>
#include <x86_64/init/init.hpp>
#include <x86_64/int/interrupt.hpp>
#include <x86_64/kernel.hpp>
#include <x86_64/stivale2.hpp>

// We need to tell the stivale bootloader where we want our stack to be.
// We are going to allocate our stack as an uninitialised array in .bss.
static uint8_t stack[4096 * 4] __attribute__((aligned(0x1000)));


// stivale2 uses a linked list of tags for both communicating TO the
// bootloader, or receiving info FROM it. More information about these tags
// is found in the stivale2 specification.

// We are now going to define a framebuffer header tag, which is mandatory when
// using the stivale2 terminal.
// This tag tells the bootloader that we want a graphical framebuffer instead
// of a CGA-compatible text mode. Omitting this tag will make the bootloader
// default to text mode, if available.
static struct stivale2_header_tag_framebuffer framebuffer_hdr_tag = {
    // Same as above.
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
        .next = 0 },
    // We set all the framebuffer specifics to 0 as we want the bootloader
    // to pick the best it can.
    .framebuffer_width = 0,
    .framebuffer_height = 0,
    .framebuffer_bpp = 32
};

// The stivale2 specification says we need to define a "header structure".
// This structure needs to reside in the .stivale2hdr ELF section in order
// for the bootloader to find it. We use this __attribute__ directive to
// tell the compiler to put the following structure in said section.
__attribute__((section(".stivale2hdr"), used)) static struct stivale2_header stivale_hdr = {
    // The entry_point member is used to specify an alternative entry
    // point that the bootloader should jump to instead of the executable's
    // ELF entry point. We do not care about that so we leave it zeroed.
    .entry_point = 0,
    // Let's tell the bootloader where our stack is.
    // We need to add the sizeof(stack) since in x86(_64) the stack grows
    // downwards.
    .stack = (uintptr_t)stack + sizeof(stack),
    // Bit 1, if set, causes the bootloader to return to us pointers in the
    // higher half, which we likely want.
    // Bit 2, if set, tells the bootloader to enable protected memory ranges,
    // that is, to respect the ELF PHDR mandated permissions for the executable's
    // segments.
    .flags = (1 << 1) | (1 << 2),
    // This header structure is the root of the linked list of header tags and
    // points to the first one in the linked list.
    .tags = (uintptr_t)&framebuffer_hdr_tag
};

// We will now write a helper function which will allow us to scan for tags
// that we want FROM the bootloader (structure tags).
void *stivale2_get_tag(struct stivale2_struct *stivale2_struct, uint64_t id) {
    struct stivale2_tag *current_tag = (stivale2_tag *)stivale2_struct->tags;
    for (;;) {
        // If the tag pointer is NULL (end of linked list), we did not find
        // the tag. Return NULL to signal this.
        if (current_tag == NULL) {
            return NULL;
        }

        // Check whether the identifier matches. If it does, return a pointer
        // to the matching tag.
        if (current_tag->identifier == id) {
            return current_tag;
        }

        // Get a pointer to the next tag in the linked list and repeat.
        current_tag = (stivale2_tag *)current_tag->next;
    }
}

void bootloader_services_init(struct stivale2_struct *handover) {
    struct stivale2_struct_tag_framebuffer *tagfb = static_cast<struct stivale2_struct_tag_framebuffer *>(stivale2_get_tag(handover, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID));
    if (tagfb == NULL) {
        for (;;)
            asm("hlt");
    }
    firefly::drivers::vbe::early_init(tagfb);
    firefly::drivers::vbe::boot_splash();
}

extern "C" [[noreturn]] void kernel_init(struct stivale2_struct *stivale2_struct) {
    bootloader_services_init(stivale2_struct);

    firefly::kernel::core::gdt::init();
    firefly::kernel::core::interrupt::init();
    firefly::kernel::main::kernel_main();
    for (;;);
}