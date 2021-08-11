#include <i386/libk++/iostream.h>

#include <i386/drivers/vga.hpp>
#include <i386/init/init.hpp>
#include <i386/init/mb2_proto.hpp>
#include <i386/kernel.hpp>

namespace firefly::kernel {
void kernel_init(mboot_param magic, [[maybe_unused]] mboot_param mb2_struct_address) {
    klog() << "Initialising i386 based kernel...\n";
    mb2proto::init(magic, mb2_struct_address);
}
}  // namespace firefly::kernel