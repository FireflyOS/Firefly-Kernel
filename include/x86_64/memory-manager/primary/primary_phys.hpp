#pragma once

#include <stdint.h>
#include "x86_64/stivale2.hpp"
#include "x86_64/libk++/bitmap.h"

namespace firefly::kernel::mm::primary {
constexpr uint32_t PAGE_SIZE = 4096;

struct contigious_allocation_result
{
    void *addr;
    struct contigious_allocation_result *next;
};

void init(struct stivale2_struct_tag_memmap *mmap);

template <typename T>
T *small_alloc(size_t base)
{
    auto index = (T*)(base + sizeof(T));
    return index;
}

}  // namespace firefly::kernel::mm::primary
