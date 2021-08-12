#include <i386/libk++/iostream.h>

#include <i386/drivers/vga.hpp>
#include <i386/init/init.hpp>
#include <i386/init/mb2_proto.hpp>

namespace firefly::kernel {
void kernel_init(mboot_param magic, mboot_param mb2_struct_address) {
    mb2proto::init(magic, mb2_struct_address);
}
}  // namespace firefly::kernel