#pragma once

#include "../multiboot2.hpp"

namespace firefly::kernel::mb2proto {
    void init(mboot_param magic, mboot_param mb2_struct_address);
}