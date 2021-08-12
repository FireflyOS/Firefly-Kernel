#include <x86_64/libk++/iostream.h>

#include <x86_64/drivers/vbe.hpp>
#include <x86_64/init/mb2_proto.hpp>
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
            }
        }
    }
}
}  // namespace firefly::kernel::mb2proto
