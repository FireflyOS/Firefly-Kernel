#include "x86_64/memory-manager/buddy_heap.hpp"

#include <utility.h>

#include "x86_64/memory-manager/buddy.hpp"


namespace firefly::kernel::mm::buddy {

bool BuddyInfoHeap::operator<(BuddyInfoHeap const& rhs) const noexcept {
    return largest_order_free < rhs.largest_order_free;
}

bool BuddyInfoHeap::operator>(BuddyInfoHeap const& rhs) const noexcept {
    return largest_order_free > rhs.largest_order_free;
}

size_t BuddyTreeHeap::size() {
    return _size;
}

size_t BuddyTreeHeap::parent(size_t index) {
    return (index - 1) / 2;
}

size_t BuddyTreeHeap::left(size_t index) {
    return (2 * index + 1);
}

size_t BuddyTreeHeap::right(size_t index) {
    return (2 * index + 2);
}

size_t BuddyTreeHeap::heapify_down(size_t i) {
    size_t left = this->left(i);
    size_t right = this->right(i);

    size_t largest = i;
    if (left < size() && base[left] > base[i]) {
        largest = left;
    }

    if (right < size() && base[right] > base[largest]) {
        largest = right;
    }

    if (largest != i) {
        firefly::std::swap(base[i], base[largest]);
        base[i].buddy->heap_index = i;
        return heapify_down(largest);
    }
    return i;
}


size_t BuddyTreeHeap::heapify_up(size_t i) {
    auto parent = this->parent(i);
    if (i && base[parent] < base[i]) {
        firefly::std::swap(base[i], base[parent]);
        base[i].buddy->heap_index = i;
        return heapify_up(parent);
    }
    return i;
}

BuddyInfoHeap* BuddyTreeHeap::push(BuddyAllocator* buddy, BuddyInfoHeap key) {
    auto node = buddy->alloc<BuddyInfoHeap>();
    node->buddy = key.buddy;
    node->largest_order_free = key.largest_order_free;
    node->buddy->heap_index = size();
    if (size() != 0) {
        node->buddy->heap_index = heapify_up(size());
    }
    _size++;
    return node;
}

BuddyInfoHeap& BuddyTreeHeap::max() {
    return base[0];
}


}  // namespace firefly::kernel::mm::buddy