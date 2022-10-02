#pragma once

#include <frg/manual_box.hpp>

#include "firefly/memory-manager/mm.hpp"

namespace firefly::kernel::mm {
class kernelHeap {
private:
    friend class frg::manual_box<kernelHeap>;
    kernelHeap() {
    }

public:
    static void init();
    VirtualAddress allocate(size_t size) const;
    void deallocate(VirtualAddress ptr) const;
};

extern constinit frg::manual_box<kernelHeap> heap;

}  // namespace firefly::kernel::mm
