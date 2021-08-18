#include <i386/libk++/iostream.h>
#include <stl/cstdlib/stdio.h>

#include <i386/drivers/vbe.hpp>
#include <i386/init/mb2_proto.hpp>
#include <i386/kernel.hpp>
#include <i386/memory-manager/bootstrap-buddy.hpp>
#include <i386/trace/strace.hpp>

namespace firefly::kernel::mb2proto {
void init(mboot_param magic, [[maybe_unused]] mboot_param mb2_struct_address) {
    /* Verify magic and alignment */
    if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
        trace::panic("Invalid magic!\n");
    }

    if (mb2_struct_address & 7) {
        trace::panic("Unaligned multiboot2 struct!\n");
    }

    unsigned size;
    size = *(unsigned *)mb2_struct_address;
    struct multiboot_tag *tag;

    for (tag = (struct multiboot_tag *)(mb2_struct_address + 8);
         tag->type != MULTIBOOT_TAG_TYPE_END;
         tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag + ((tag->size + 7) & ~7))) {
        switch (tag->type) {
            case MULTIBOOT_TAG_TYPE_FRAMEBUFFER: {
                firefly::drivers::vbe::early_init(reinterpret_cast<struct multiboot_tag_framebuffer *>(tag));
                firefly::kernel::main::write_ff_info();
                    break;
            }
        }
    }

    for (tag = (struct multiboot_tag *)(mb2_struct_address + 8);
         tag->type != MULTIBOOT_TAG_TYPE_END;
         tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag + ((tag->size + 7) & ~7))) {
        switch (tag->type) {
            case MULTIBOOT_TAG_TYPE_MMAP: {
                multiboot_memory_map_t *mmap = reinterpret_cast<struct multiboot_tag_mmap *>(tag)->entries;
                firefly::mm::buddy::bootstrap_buddy(mmap->addr, mmap->len);

                printf("mmap\n");
                for (;
                     (multiboot_uint8_t *)mmap < (multiboot_uint8_t *)tag + tag->size;
                     mmap = (multiboot_memory_map_t *)((unsigned long)mmap + ((struct multiboot_tag_mmap *)tag)->entry_size))
                    printf (" base_addr = 0x%x,"
                      " length = 0x%x, type = 0x%x\n",
                      (unsigned) (mmap->addr & 0xFFFFFFFF),
                      (unsigned) (mmap->len & 0xFFFFFFFF),
                      (unsigned) mmap->type);
                break;
            }
        }
    }
    printf("MBI Size: %d\n", size);
}
}  // namespace firefly::kernel::mb2proto