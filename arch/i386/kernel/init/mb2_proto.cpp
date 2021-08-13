#include <i386/libk++/iostream.h>
#include <stl/cstdlib/stdio.h>

#include <i386/drivers/vbe.hpp>
#include <i386/init/mb2_proto.hpp>
#include <i386/trace/strace.hpp>

namespace firefly::kernel::mb2proto {
void init(mboot_param magic, [[maybe_unused]] mboot_param mb2_struct_address) {
    /* Verify magic and alignment */
    if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
        panic("Invalid magic!\n");
    }

    if (mb2_struct_address & 7) {
        panic("Unaligned multiboot2 struct!\n");
    }

    unsigned size;
    size = *(unsigned *)mb2_struct_address;
    struct multiboot_tag *tag;
    printf("MBI Size: %d\n", size);

    for (tag = (struct multiboot_tag *)(mb2_struct_address + 8);
         tag->type != MULTIBOOT_TAG_TYPE_END;
         tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag + ((tag->size + 7) & ~7))) {
        switch (tag->type) {
            case MULTIBOOT_TAG_TYPE_FRAMEBUFFER: {
                firefly::drivers::vbe::early_init(reinterpret_cast<struct multiboot_tag_framebuffer*>(tag));
            }
        }
    }
}
}  // namespace firefly::kernel::mb2proto