#pragma once

#include <stdint.h>

#include "x86_64/compiler/clang++.hpp"
#include "x86_64/libk++/bitmap.h"
#include "x86_64/stivale2.hpp"

namespace firefly::kernel::mm::primary {
constexpr uint32_t PAGE_SIZE = 4096;

typedef struct
{
    void **data;  // The array of pointers allocated - NOTE: These are NOT guaranteed to be contigious physical pages
    size_t count; // Number of pages allocated
} primary_res_t;

void init(struct stivale2_struct_tag_memmap *mmap);
primary_res_t *allocate(size_t pages);
primary_res_t *daemon_late_alloc(size_t pages);
void deallocate(primary_res_t *allocation_structure);

}  // namespace firefly::kernel::mm::primary
