#pragma once

#include "firefly/stivale2.hpp"

namespace firefly::kernel::device::stivale2_term
{
    void init(stivale2_struct_tag_terminal* term);
    void write(const char *str);
    uint64_t term_write_addr();
} // namespace firefly::kernel::device::stivale2_term
