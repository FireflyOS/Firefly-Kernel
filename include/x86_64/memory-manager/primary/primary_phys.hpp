#pragma once

#include <stdint.h>
#include "x86_64/stivale2.hpp"

namespace firefly::kernel::mm::primary {
struct free_list {
    uint32_t *bitmap;
};

struct used_list {
    uint32_t *bitmap;
};

struct primary {
    struct free_list *free;
    struct used_list *used;
};

void init(struct stivale2_struct_tag_memmap *mmap);

}  // namespace firefly::kernel::mm::primary
