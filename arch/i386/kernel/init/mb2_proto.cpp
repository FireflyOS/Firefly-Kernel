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
    klog("Magic is valid\n");

    if (mb2_struct_address & 7) {
        panic("Unaligned multiboot2 struct!\n");
    }
    klog("Multiboot2 struct is aligned\n");

    unsigned size;
    size = *(unsigned *)mb2_struct_address;
    struct multiboot_tag *tag;
    printf("MBI Size: %d\n", size);

    for (tag = (struct multiboot_tag *)(mb2_struct_address + 8);
         tag->type != MULTIBOOT_TAG_TYPE_END;
         tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag + ((tag->size + 7) & ~7))) {
        printf("Type: %d | Size: %d\n", tag->type, tag->size);
    }
}
}  // namespace firefly::kernel::mb2proto
