#include <i386/libk++/iostream.h>
#include <stl/cstdlib/stdio.h>

#include <i386/init/mb2_proto.hpp>
#include <i386/trace/strace.hpp>

namespace firefly::kernel::mb2proto {
void init(mboot_param magic, [[maybe_unused]] mboot_param mb2_struct_address) {
    /* Verify magic and alignment */
    if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
        panic("Invalid magic!\n");
    }
    klog() << "Magic is valid\n";

    if (mb2_struct_address & 7) {
        panic("Unaligned multiboot2 struct!\n");
    }
    klog() << "Multiboot2 struct is aligned\n";

    unsigned size;
    size = *(unsigned *)mb2_struct_address;
    struct multiboot_tag *tag;
    printf("MBI Size: %d\n", size);

    for (tag = (struct multiboot_tag *)(mb2_struct_address + 8);
         tag->type != MULTIBOOT_TAG_TYPE_END;
         tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag + ((tag->size + 7) & ~7))) {
        switch (tag->type) {
            case MULTIBOOT_TAG_TYPE_VBE: {
                // struct multiboot_tag_vbe *vbe = (struct multiboot_tag_vbe*)tag;
                // vbe->vbe_mode_info
            }
            // case MULTIBOOT_TAG_TYPE_FRAMEBUFFER: {
            //     multiboot_uint32_t color;

            //     // unsigned i;
            //     struct multiboot_tag_framebuffer *tagfb = (struct multiboot_tag_framebuffer *)tag;
            //     // void *fb = (void *)(unsigned long)tagfb->common.framebuffer_addr;
            //     printf("FB Address: 0x%x\n", tagfb->common.framebuffer_addr);
            //     switch (tagfb->common.framebuffer_type) {
            //         case MULTIBOOT_FRAMEBUFFER_TYPE_RGB:
            //             // color = ((1 << tagfb->framebuffer_blue_mask_size) - 1)
            //             //         << tagfb->framebuffer_blue_field_position;
            //             break;

            //         case MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT:
            //             color = '\\' | 0x0100;
            //             break;

            //         default:
            //             color = 0xffffffff;
            //             break;
            //     }

            //     printf("Width: %d | Height: %d\n", tagfb->common.framebuffer_width, tagfb->common.framebuffer_height);
            //     break;
            // }
        }
    }
}
}  // namespace firefly::kernel::mb2proto
// }
// }  // namespace firefly::kernel::mb2proto
