#include <x86_64/libk++/iostream.h>

namespace firefly::libkern {
firefly::drivers::vga::cursor& get_cursor_handle() {
    return firefly::kernel::main::internal_cursor_handle();
}

void globalize_vga_writer(firefly::drivers::vga::cursor& crs) {
    cout = crs;
}
}  // namespace firefly::libkern
