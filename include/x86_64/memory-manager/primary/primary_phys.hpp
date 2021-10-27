#pragma once

#include <stdint.h>
#include "x86_64/stivale2.hpp"
#include "x86_64/libk++/bitmap.h"

namespace firefly::kernel::mm::primary {
constexpr uint32_t PAGE_SIZE = 4096;

struct free_list {
    libkern::Bitmap bitmap_freelist;
    uint32_t *bitmap;
};

struct used_list {
    libkern::Bitmap bitmap_usedlist;
    uint32_t *bitmap;
};

struct primary {
    struct free_list *free;
    struct used_list *used;
};

void init(struct stivale2_struct_tag_memmap *mmap);

}  // namespace firefly::kernel::mm::primary
