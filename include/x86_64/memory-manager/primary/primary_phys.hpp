#pragma once

#include <stdint.h>
#include "x86_64/stivale2.hpp"
#include "x86_64/libk++/bitmap.h"

namespace firefly::kernel::mm::primary {
constexpr uint32_t PAGE_SIZE = 4096;
constexpr intptr_t MAGIC = 0xC0FFEE; // Linked list header signature, this must be skipped (done for you by unpack())

struct primary_allocation_result
{
    const void *addr;
    struct primary_allocation_result *next;
    struct primary_allocation_result *unpack()
    {
        // Check fo invalid magic (i.e. `this` points somewhere that is *not* the head)
        if (this->addr != reinterpret_cast<void*>(MAGIC))
            return nullptr;

        return this->next;
    }
};

void init(struct stivale2_struct_tag_memmap *mmap);
struct primary_allocation_result *allocate(size_t pages);
void deallocate(struct primary_allocation_result *list);

}  // namespace firefly::kernel::mm::primary
