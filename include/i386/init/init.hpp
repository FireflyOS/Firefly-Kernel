#pragma once

#include <i386/multiboot2.hpp>

namespace firefly::kernel {
    void kernel_init(mboot_param magic, mboot_param mb2_struct_address);
}