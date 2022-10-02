#pragma once

#include "firefly/memory-manager/secondary/heap.hpp"

struct Allocator {
    VirtualAddress allocate(size_t size) const {
        using namespace firefly::kernel::mm;
        return heap->allocate(size);
    }

    void deallocate(VirtualAddress ptr) const {
        using namespace firefly::kernel::mm;
        return heap->deallocate(ptr);
    }

    void free(VirtualAddress ptr) const {
        deallocate(ptr);
    }
};