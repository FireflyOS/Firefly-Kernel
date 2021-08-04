#include <drivers/vga.hpp>
#include <init/init.hpp>
#include <init/mb2_proto.hpp>
#include <kernel.hpp>

namespace firefly::kernel {
void kernel_init([[maybe_unused]]uint64_t *mb2_struct_address) {
    using firefly::drivers::vga::color;
    using firefly::drivers::vga::cursor;
    
    cursor& crs = kernel::main::get_cursor();
    crs << "Testing globalized vga writer...\n";
    mb2proto::init(mb2_struct_address);
}
}  // namespace firefly::kernel