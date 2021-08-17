#include "i386/pmm/buddy.hpp"

#include <cstdlib/cmath.h>
#include <utility.h>

bool Chunk::can_allocate(uint8_t order) const noexcept {
    if (order > MAXIMUM_ORDER) {
        return false;
    }
    return free_values[order] || can_allocate(order + 1);
}

bool BuddyNode::is_right() const noexcept {
    return _is_right;
}

BuddyNode* BuddyNode::get_matching_buddy() {
    if (order == MAXIMUM_ORDER) {
        return nullptr;
    }
    // if is_right -> to the left
    // else -> to the right
    auto addition = -_is_right * (pow(2, order) - 1);
    return this + addition;
}

BuddyNode* BuddyNode::get_parent() {
    if (order == MAXIMUM_ORDER) {
        return nullptr;
    }
    // if is_right -> this - pow(2, order + 1)
    // else this - 1
    auto offset = 1;
    if (is_right()) {
        offset = pow(2, order + 1);
    }
    return this - offset;
}

BuddyNode* BuddyNode::get_left_child() {
    if (order == 0) {
        return nullptr;
    }
    return this + 1;
}

BuddyNode* BuddyNode::get_right_child() {
    if (order == 0) {
        return nullptr;
    }
    return this + pow(2, order);
}

BuddyNode* lambda(BuddyNode* node, BuddyAllocator* buddy, uint8_t _order) {
    if (_order == node->order && is_free() && !is_split()) {
        return node;
    }

    auto left = lambda(node->left_split, buddy, _order);
    if (left) {
        return left->get_parent();
    }

    auto right = lambda(node->right_split, buddy, _order);
    if (right) {
        return right->get_parent();
    }

    // neither right nor left is found, we gotta split
    auto to_split_left = lambda(node->left_split, buddy, _order - 1);
    if (to_split_left) {
        to_split_left->split(buddy);
        return to_split_left->get_left_child();
    }

    auto to_split_right = lambda(node->right_split, buddy, _order - 1); 
    if (to_split_right) {
        to_split_right->split(buddy);
        return to_split_right->get_left_child();
    }

    return nullptr; // we couldn't find an already existing node
    // nor split a node, make sure to check heap layout please if this happens
}

// This function returns the _parent_ node that should be allocated from
// however if this returns an order MAXIMUM_ORDER node
// then this node should just be allocated
// if nullptr is returned then no viable candidate was found
// if you did check can_allocate then consider
// checking the implementaiton of can_allocate.
BuddyNode* Chunk::get_free_buddy(BuddyAllocator* buddy, uint8_t order) noexcept {
    if (order > MAXIMUM_ORDER) {
        return nullptr;
    }

    if (order == MAXIMUM_ORDER) {
        return &root;
    }

    return lambda(&root)->get_parent();
}


size_t BuddyNode::pointed_size() const noexcept {
    return SMALLEST_CHUNK * pow(2, order);
}

bool BuddyNode::is_split() const noexcept {
    return _is_split;
}

bool BuddyNode::is_taken() const noexcept {
    return _is_taken;
}

// if the taken_until pointer is null and this node isn't split
// that means that this node hasn't yet been allocated from
bool BuddyNode::is_free() const noexcept {
    return !is_split() && !is_taken();
}

void BuddyNode::split(BuddyAllocator* buddy) noexcept {
    _is_split = true;
    auto chunk = to_chunk(buddy);
    if (order == 0) {
        // panic; can't split a zero-order node
    }
    chunk->free_values[order] -= 1;
    chunk->free_values[order + 1] += 2;
}

void BuddyNode::merge_buddy(BuddyAllocator* buddy) noexcept {
    parent->_is_split = false;
    auto chunk = to_chunk(buddy);
    if (order == MAXIMUM_ORDER) {
        // can't merge a maximum order node
    }
    // -= 1 because the other one has already been added when it was deallocated
    chunk->free_values[order] += 1;
}

Chunk* BuddyNode::to_chunk(BuddyAllocator* buddy) const noexcept {
    return buddy->chunk_for(physical_addr);
}

bool BuddyInfoHeap::operator<(BuddyInfoHeap const& rhs) const noexcept {
    return largest_order_free < rhs.largest_order_free;
}

bool BuddyInfoHeap::operator>(BuddyInfoHeap const& rhs) const noexcept {
    return largest_order_free > rhs.largest_order_free;
}

BuddyAllocator::BuddyAllocator(void* base_address)
    : base_address{ reinterpret_cast<Chunk*>(base_address) } {
}

size_t BuddyAllocator::calculate_nodes_for_max_order() {
    size_t result = 0;
    int order = MAXIMUM_ORDER;
    size_t current_tier_count = 1;
    // For every tier of nodes we need
    do {
        result += current_tier_count;
        current_tier_count *= 2;
        order--;
    } while (order >= 1);

    // Now the state array
    return result;
}

size_t BuddyAllocator::estimate_memory_used(size_t address_range) {
    size_t trees_count = (address_range / LARGEST_CHUNK);
    auto trees_size = calculate_nodes_for_max_order() * trees_count * (sizeof(BuddyNode) + sizeof(Chunk));
    auto heap_size = trees_count * sizeof(BuddyInfoHeap);
    return trees_size + heap_size;
}

// optional somehow
int8_t BuddyAllocator::order_for(size_t bytes) noexcept {
    if (bytes > LARGEST_CHUNK) {
        return -1;
    }
    int8_t result = 0;  // default with an order 0 node, we start with smallest
    while (SMALLEST_CHUNK * pow(2, result) < bytes) {
        result++;
    }
    return result;
}

Chunk* BuddyAllocator::chunk_for(void* address) {
    size_t addr = reinterpret_cast<size_t>(address);
    return &base_address[addr / MAXIMUM_ORDER];
}
/*
 * Initializes the BuddyNode[] @ base_address, where the nodes describe
 * the structure of memory_base address until memory_base + memory_available
 *
 * char* so we get mathematical operations on the pointer
 * 
 * Creation order:
 *         0
 *        / \
 *       1   4
 *      / \ / \
 *     2  3 5  6
 * The reason get_parent() and get_buddY() work is becuase of creation strucutre
 * cuz it means that in memory they'll be laid out like
 * 0 1 2 3 4 5 6
 */
void BuddyAllocator::initialize(size_t memory_available, char* memory_base) {
    // NOTE: any memory that's exceeds a multiple of LARGEST_CHUNK will be LOST and not used.
    size_t zero_nodes_needed = memory_available / LARGEST_CHUNK;
    for (size_t i = 0; i < zero_nodes_needed; i++) {
        auto node = alloc_chunk();
        node->root.physical_addr = memory_base + (i * LARGEST_CHUNK);
        node->root.order = MAXIMUM_ORDER;
        node->free_values[MAXIMUM_ORDER] = 1;  // only an order 4 node is available.
        auto left_child = alloc_buddy_node();
        auto right_child = alloc_buddy_node();
        left_child->order = 1;
        right_child->order = 1;

        // zero-node has been created, create all the child nodes that we need.
        create_tree_structure(left_child);
        create_tree_structure(right_child);
    }

    // finalize, aka construct the heap.
    for (size_t i = 0; i < zero_nodes_needed; i++) {
        buddy_heap.push(this, BuddyInfoHeap{
                                  reinterpret_cast<Chunk*>(memory_base) + i,
                                  MAXIMUM_ORDER });
    }
}

void BuddyAllocator::create_tree_structure(BuddyNode* parent_node) {
    uint8_t order = parent_node->order - 1;
    auto one = alloc_buddy_node();
    auto two = alloc_buddy_node();
    one->order = order;

    two->order = order;
    two->_is_right = true;

    // Say parent_node covers physical addresses 0 through 16, then one would point to 0, two would point to 8
    // they would both have 50% of the memory of the parent.
    one->physical_addr = parent_node->physical_addr;
    // char is 1 byte.
    // Since we know that MAXIMUM is divisibe by 2 (since it's a power of 2) we can just floor the address division.
    two->physical_addr = reinterpret_cast<char*>(parent_node->physical_addr) + (parent_node->pointed_size() / 2);
    if (order > 0) {
        create_tree_structure(one);
        create_tree_structure(two);
    }
}

BuddyNode* BuddyAllocator::alloc_buddy_node() {
    auto temp = reinterpret_cast<BuddyNode*>(base_address);
    *temp = BuddyNode{};
    (*reinterpret_cast<BuddyNode**>(&base_address))++;
    return temp;
}

Chunk* BuddyAllocator::alloc_chunk() {
    // Decrement the address since our array grows down.
    auto temp = base_address++;
    *temp = Chunk{};  // default construct it so all fields are 0
    return temp;
}

BuddyInfoHeap* BuddyAllocator::alloc_heap_node() {
    auto temp = reinterpret_cast<BuddyInfoHeap*>(base_address);
    *temp = BuddyInfoHeap{};
    (*reinterpret_cast<BuddyInfoHeap**>(&base_address))++;
    return temp;
}

void* BuddyAllocator::allocate(uint8_t order) {
    auto& max = buddy_heap.max();
    if (!max.buddy->can_allocate(order)) {
        return nullptr;
    }
    auto buddy = max.buddy->get_free_buddy(this, order);
    auto physical_addr = nullptr;
    if (order == MAXIMUM_ORDER) {
        buddy->_is_taken = true;
        physical_addr = buddy->physical_addr;
    } else {
        auto left = buddy->get_left_child();
        void* physical_addr = nullptr;
        if (left->is_free()) {
            left->_is_taken = true;
            physical_addr = left->physical_addr;
        } else {
            auto right = buddy->get_right_child();
            right->_is_taken = true;
            physical_addr = right->physical_addr;
        }
    }
    Chunk* chunk = this->chunk_for(physical_addr);
    chunk->free_values[order]--;
    chunk->fix_heap(this);
    return physical_addr;
}

// true if it's the left one
firefly::std::pair<BuddyNode*, bool> deallocate_find(void* addr, BuddyNode* buddy, uint8_t order) {
    if (buddy->physical_addr == addr && order == buddy->order) {
        return buddy;
    }

    if (buddy->get_left_child() == nullptr) {
        return nullptr;
    }

    auto one = deallocate_find(addr, buddy->get_left_child());
    if (one) {
        return one;
    }

    auto two = deallocate_find(addr, buddy->get_right_child());
    if (two) {
        return one;
    }

    return nullptr;
}

void BuddyAllocator::deallocate(void* addr, uint8_t order) {
    auto chunk = chunk_for(addr);
    if (order == MAXIMUM_ORDER) {
        chunk->root->_is_taken = false;
        chunk->root->_is_split = false;
        firefly::std::fill(chunk->free_values.begin(), chunk->free_values.end(), 0);
        chunk->free_values[MAXIMUM_ORDER] = 1;
    }

    auto& [allocated_buddy, is_left] = deallocate_find(chunk->root, this, order);
    if (!allocated_buddy) {
        return nullptr;
    }
    auto other = allocated_buddy->get_matching_buddy();
    if (other->is_free()) {
        other->get_parent()->merge_buddy(this);
    }

    chunk->free_values[order]--;
    chunk->fix_heap();
}

void Chunk::fix_heap(BuddyAllocator* buddy) {
    auto max = firefly::std::max_element(free_values.begin(), free_values.end());
    uint8_t max_order = -1;
    if (max != free_values.end()) {
        max_order = *max;
    }

    auto heap_element = buddy->heap_index(heap_index);
    auto before = heap_element.largest_order_free;
    if (max_order == before) {
        // we merged / split but the maximum available size didn't chagne
        return;
    }
    heap_element.largest_order_free = max_order;
    if (max_order > before) {
        heap_element->heapify_up(heap_index);
    } else {
        heap_element->heapify_down(heap_index);
    }
}

BuddyInfoHeap* BuddyAllocator::heap_index(size_t idx) {
    return buddy_heap.base[idx];
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

void BuddyTreeHeap::heapify_down(size_t i) {
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
        firefly::std::swap(base[i].buddy->heap_index, base[largest].buddy->heap_index);
        heapify_down(largest);
    }
}

void BuddyTreeHeap::heapify_up(size_t i) {
    auto parent = this->parent(i);
    if (i && base[parent] < base[i]) {
        firefly::std::swap(base[i], base[parent]);
        firefly::std::swap(base[i].buddy->heap_index, base[parent].buddy->heap_index);
        heapify_up(parent);
    }
}

BuddyInfoHeap* BuddyTreeHeap::push(BuddyAllocator* buddy, BuddyInfoHeap key) {
    auto node = buddy->alloc_heap_node();
    *node = key;
    size_t index = size() - 1;
    heapify_up(index);
    return node;
}

BuddyInfoHeap& BuddyTreeHeap::max() {
    return base[0];
}