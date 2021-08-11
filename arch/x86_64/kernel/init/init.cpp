#include <x86_64/libk++/iostream.h>

#include <x86_64/drivers/vga.hpp>
#include <x86_64/init/init.hpp>
#include <x86_64/init/mb2_proto.hpp>

namespace firefly::kernel {
void kernel_init(uint64_t magic, [[maybe_unused]] uint64_t mb2_struct_address) {
    klog() << "Initialising x86_64 based kernel...\n";
    mb2proto::init(magic, mb2_struct_address);
}
}  // namespace firefly::kernel