#pragma once

namespace firefly::kernel::main {
[[noreturn]] void kernel_main(stivale2_struct *handover);
void write_ff_info();
}