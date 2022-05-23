#include "x86_64/fb/stivale2-term.hpp"
#include <stl/cstdlib/cstring.h>
#include <stddef.h>

namespace firefly::kernel::device::stivale2_term
{
    void (*term_write)(const char *string, size_t length);
    static uint64_t entry_point = 0;

    void init(stivale2_struct_tag_terminal* term)
    {
        term_write = (void(*)(const char *, size_t))term->term_write;
        entry_point = term->term_write;
    }

    void write(const char *str)
    {
        term_write(str, strlen(str));
    }

    uint64_t term_write_addr()
    {
        return entry_point;
    }
} // firefly::kernel::device::stivale2_term