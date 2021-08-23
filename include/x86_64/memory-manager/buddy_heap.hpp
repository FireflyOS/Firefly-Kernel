#ifndef __BUDDY_HEAP_HPP__
#define __BUDDY_HEAP_HPP__

#include <cstdlib/cstdint.h>

namespace firefly::kernel::mm::buddy {
/**
 * The heap is what's used to find a free node
 * Every order MAXIMUM_ORDER node has information in a `Chunk`
 * It keeps track of where it's currently located in the heap AND
 * how many of each order are free, the largest free order is updated in 
 * BuddyInfoHeap, which matches the chunk and has a pointer to it.
 * The heap should be updated every time add_order is called on a chunk
 * (fix_heap())
 */


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