#include "firefly/init/init.hpp"

#include <stddef.h>
#include <stdint.h>

#include "firefly/console/stivale2-term.hpp"
#include "firefly/intel64/gdt/gdt.hpp"
#include "firefly/intel64/gdt/tss.hpp"
#include "firefly/intel64/int/interrupt.hpp"
#include "firefly/kernel.hpp"
#include "firefly/memory-manager/primary/primary_phys.hpp"
#include "firefly/panic.hpp"
#include "firefly/stivale2.hpp"

// We need to tell the stivale bootloader where we want our stack to be.
// We are going to allocate our stack as an uninitialized array in .bss.
static uint8_t stack[4096 * 2] __attribute__((aligned(0x1000)));

// stivale2 uses a linked list of tags for both communicating TO the
// bootloader, or receiving info FROM it. More information about these tags
// is found in the stivale2 specification.

// We are now going to define a framebuffer header tag, which is mandatory when
// using the stivale2 terminal.
// This tag tells the bootloader that we want a graphical framebuffer instead
// of a CGA-compatible text mode. Omitting this tag will make the bootloader
// default to text mode, if available.
static stivale2_header_tag_framebuffer framebuffer_hdr_tag = {
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

struct stivale2_header_tag_terminal terminal_hdr_tag = {
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_TERMINAL_ID,
        .next = (uint64_t)&framebuffer_hdr_tag },
    .flags = 0
};

// The stivale2 specification says we need to define a "header structure".
// This structure needs to reside in the .stivale2hdr ELF section in order
// for the bootloader to find it. We use this __attribute__ directive to
// tell the compiler to put the following structure in said section.
__attribute__((section(".stivale2hdr"), used)) static stivale2_header stivale_hdr = {
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
    // Bit 3, if set, enables fully virtual kernel mappings, which we want as
    // they allow the bootloader to pick whichever *physical* memory address is
    // available to load the kernel, rather than relying on us telling it where
    // to load it.
    // Bit 4 disables a deprecated feature and should always be set.
    .flags = (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4),
    // This header structure is the root of the linked list of header tags and
    // points to the first one in the linked list.
    .tags = (uintptr_t)&terminal_hdr_tag
};

// We will now write a helper function which will allow us to scan for tags
// that we want FROM the bootloader (structure tags).#include <cstdlib/#include <cstdint>>
void* stivale2_get_tag(stivale2_struct* stivale2_struct, uint64_t id) {
    stivale2_tag* current_tag = (stivale2_tag*)stivale2_struct->tags;
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
        current_tag = (stivale2_tag*)current_tag->next;
    }
}

#include "firefly/memory-manager/zone-specifier.hpp"

using namespace firefly::kernel;

void bootloader_services_init(stivale2_struct* handover) {
    auto tag_term = static_cast<stivale2_struct_tag_terminal*>(stivale2_get_tag(handover, STIVALE2_STRUCT_TAG_TERMINAL_ID));
    if (tag_term == NULL) {
        for (;;)
            asm("hlt");
    }

    firefly::kernel::device::stivale2_term::init(tag_term);

    log_core_firefly_contributors();  // This is here to make sure it's the first message being logged

    auto tagmem = static_cast<stivale2_struct_tag_memmap*>(stivale2_get_tag(handover, STIVALE2_STRUCT_TAG_MEMMAP_ID));
    if (tagmem == NULL) {
        firefly::panic("Cannot obtain memory map");
    }
    firefly::kernel::mm::pmm::init(tagmem);
}

extern "C" [[noreturn]] void kernel_init(stivale2_struct* handover) {
    firefly::kernel::core::gdt::init();
    firefly::kernel::core::tss::core0_tss_init(reinterpret_cast<size_t>(stack));
    firefly::kernel::core::interrupt::init();

    bootloader_services_init(handover);

    firefly::kernel::kernel_main();
    __builtin_unreachable();
}
