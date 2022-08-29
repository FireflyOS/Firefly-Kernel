#include "firefly/init/init.hpp"

#include <stddef.h>
#include <stdint.h>

#include "firefly/intel64/cpu/cpu.hpp"
#include "firefly/intel64/int/interrupt.hpp"
#include "firefly/kernel.hpp"
#include "firefly/memory-manager/primary/primary_phys.hpp"
#include "firefly/memory-manager/virtual/virtual.hpp"
#include "firefly/stivale2.hpp"

// We need to tell the stivale bootloader where we want our stack to be.
// We are going to allocate our stack as an uninitialized array in .bss.
static uint8_t stack[PAGE_SIZE * 2] __attribute__((aligned(PAGE_SIZE)));

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
    .stack = reinterpret_cast<uintptr_t>(stack) + sizeof(stack),
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
    .tags = 0
};

// We will now write a helper function which will allow us to scan for tags
// that we want FROM the bootloader (structure tags).#include <cstdlib/#include <cstdint>>
void* stivale2_get_tag(stivale2_struct* stivale2_struct, uint64_t id) {
    stivale2_tag* current_tag = reinterpret_cast<stivale2_tag*>(stivale2_struct->tags);
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
        current_tag = reinterpret_cast<stivale2_tag*>(current_tag->next);
    }
}

void bootloader_services_init(stivale2_struct* handover) {
    using namespace firefly::kernel;
    const auto verify = [](auto tag) {
        if (tag == NULL) {
            for (;;)
                asm("hlt");
        }
        return tag;
    };

    auto tagmem = verify(static_cast<stivale2_struct_tag_memmap*>(stivale2_get_tag(handover, STIVALE2_STRUCT_TAG_MEMMAP_ID)));

    core::paging::bootMapExtraRegion(tagmem);
    mm::Physical::init(tagmem);
    mm::kernelPageSpace::init();
}

extern "C" [[noreturn]] void kernel_init(stivale2_struct* handover) {
    firefly::kernel::initializeThisCpu(reinterpret_cast<uint64_t>(stack));
    firefly::kernel::core::interrupt::init();

    bootloader_services_init(handover);

    firefly::kernel::kernel_main();
    __builtin_unreachable();
}
