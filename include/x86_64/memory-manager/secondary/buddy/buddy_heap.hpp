#ifndef __BUDDY_HEAP_HPP__
#define __BUDDY_HEAP_HPP__

#include <cstdlib/cstdint.h>

namespace firefly::kernel::mm::buddy {

struct Chunk;
class BuddyAllocator;

struct BuddyInfoHeap {
    Chunk* buddy = nullptr;
    int8_t largest_order_free = -1;

    bool operator<(BuddyInfoHeap const& rhs) const noexcept;
    bool operator>(BuddyInfoHeap const& rhs) const noexcept;
};


struct BuddyTreeHeap {
    BuddyInfoHeap* base = nullptr;
    size_t _size = 0;

    size_t size();

    size_t parent(size_t index);

    size_t left(size_t index);

    size_t right(size_t index);

    size_t heapify_down(size_t i);

    size_t heapify_up(size_t i);

    BuddyInfoHeap* push(BuddyAllocator* buddy, BuddyInfoHeap key);

    BuddyInfoHeap& max();
};
}  // namespace firefly::kernel::mm::buddy
#endif