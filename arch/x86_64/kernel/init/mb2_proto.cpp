#include <stl/cstdlib/stdio.h>

#include <x86_64/drivers/vbe.hpp>
#include <x86_64/init/mb2_proto.hpp>
#include <x86_64/memory-manager/bootstrap-buddy.hpp>
#include <x86_64/memory-manager/relocation.hpp>
#include <x86_64/multiboot2.hpp>


namespace firefly::kernel::mb2proto {
void init(uint64_t magic, [[maybe_unused]] uint64_t mb2_struct_address) {
    if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
        for (;;)
            asm("cli;hlt");
    }

    unsigned size;
    size = *(unsigned *)mb2_struct_address;
    struct multiboot_tag *tag;

    for (tag = (struct multiboot_tag *)(mb2_struct_address + 8);
         tag->type != MULTIBOOT_TAG_TYPE_END;
         tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag + ((tag->size + 7) & ~7))) {
        switch (tag->type) {
            case MULTIBOOT_TAG_TYPE_FRAMEBUFFER: {
                // firefly::drivers::vbe::early_init(reinterpret_cast<struct multiboot_tag_framebuffer *>(tag));
                break;
            }

            case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO: {
                printf("mem_lower = %dKB, mem_upper = %dKB\n",
                       ((struct multiboot_tag_basic_meminfo *)tag)->mem_lower,
                       ((struct multiboot_tag_basic_meminfo *)tag)->mem_upper);
                break;
            }

            case MULTIBOOT_TAG_TYPE_BOOTDEV: {
                printf("Boot device 0x%x, slice %d, part %d\n",
                       ((struct multiboot_tag_bootdev *)tag)->biosdev,
                       ((struct multiboot_tag_bootdev *)tag)->slice,
                       ((struct multiboot_tag_bootdev *)tag)->part);
                break;
            }

            case MULTIBOOT_TAG_TYPE_MMAP: {
                using firefly::mm::relocation::conversion::to_higher_half;
                multiboot_memory_map_t *mmap = reinterpret_cast<struct multiboot_tag_mmap *>(tag)->entries;
                firefly::mm::buddy::bootstrap_buddy(mmap->addr, mmap->len);

                printf("mmap\n");
                for (;
                     (multiboot_uint8_t *)mmap < (multiboot_uint8_t *)tag + tag->size;
                     mmap = (multiboot_memory_map_t *)((unsigned long)mmap + ((struct multiboot_tag_mmap *)tag)->entry_size))
                    printf(
                        " base_addr = 0x%x,"
                        " length = 0x%x, type = 0x%x\n",
                        to_higher_half(mmap->addr),
                        mmap->len,
                        mmap->type);
                break;
            }
        }
    }
}
}  // namespace firefly::kernel::mb2proto