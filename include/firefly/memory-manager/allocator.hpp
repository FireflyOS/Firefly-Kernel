#pragma once

#include "firefly/logger.hpp"
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

    void deallocate(VirtualAddress ptr, [[maybe_unused]] long unsigned int sz) const {
        using namespace firefly::kernel::mm;
        return heap->deallocate(ptr);
    }

    void free(VirtualAddress ptr) const {
        // firefly::logLine << firefly::fmt::hex << (uintptr_t) ptr << "\n" << firefly::fmt::endl;
        deallocate(ptr);
    }
};
