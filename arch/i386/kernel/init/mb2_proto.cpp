#include <i386/libk++/iostream.h>

#include <i386/init/mb2_proto.hpp>

namespace firefly::kernel::mb2proto {
void init(mboot_param magic, [[maybe_unused]] mboot_param mb2_struct_address) {
    if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
        klog("Invalid magic!\n");
    } else {
        klog("Magic is valid\n");
    }
}
}  // namespace firefly::kernel::mb2proto
