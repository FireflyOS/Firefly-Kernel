#include <stl/iostream.h>

#include <drivers/vga.hpp>
#include <init/init.hpp>
#include <init/mb2_proto.hpp>

namespace firefly::kernel {
void kernel_init([[maybe_unused]] uint64_t *mb2_struct_address) {
    mb2proto::init(mb2_struct_address);
}
}  // namespace firefly::kernel