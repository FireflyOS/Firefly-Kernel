#include <x86_64/libk++/iostream.h>

#include <x86_64/init/mb2_proto.hpp>
#include <x86_64/multiboot2.hpp>

namespace firefly::kernel::mb2proto {
void init(uint64_t magic, [[maybe_unused]] uint64_t mb2_struct_address) {
    if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
        klog("Invalid magic!\n");
    } else {
        klog("Magic is valid\n");
    }
}
}  // namespace firefly::kernel::mb2proto
