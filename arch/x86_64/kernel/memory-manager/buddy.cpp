#include "x86_64/memory-manager/buddy.hpp"

#include <algorithm.h>
#include <utility.h>

#define assert(x) (void)(x)

namespace firefly::kernel::mm::buddy {

// For future debugging purposes
// void printBT(const std::string& prefix, BuddyNode* node, bool isLeft)
// {
// 	if (node != nullptr)
// 	{
// 		std::cout << prefix;

// 		std::cout << (isLeft ? "|--" : "\\__");

// 		// print the value of the node
// 		std::cout <<
// 			" order: " << int(node->order) <<
// 			" free: " << bool(!node->is_taken()) <<
// 			" split: " << bool(node->is_split())

// 			<< std::endl;

// 		// enter the next tree level - left and right branch
// 		printBT(prefix + (isLeft ? "|   " : "    "), node->get_left_child(), true);
// 		printBT(prefix + (isLeft ? "|   " : "    "), node->get_right_child(), false);
// 	}
// }

// void printBT(BuddyNode* node)
// {
// 	printBT("", node, false);
// }

// void print(BuddyTreeHeap* node)
// {
// 	size_t arrlen = node->size();
// 	BuddyInfoHeap* arr = node->base;
// 	std::cout << (int)arr[0].largest_order_free << std::endl;
// 	for (int i = 1; i < arrlen; i++)
// 	{
// 		std::cout << (int)arr[i].largest_order_free << " ";
// 		if (i == pow(2, i - 1))
// 		{
// 			std::cout << std::endl;
// 		}
// 	}
// 	std::cout << std::endl;
// }

bool Chunk::can_allocate(uint8_t order) const noexcept {
    if (!can_be_allocated) {
        return false;
    }

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
    // to left
    // 1 3 7 15
    // to right
    // 1 3 7 15
    // 2 ** 0 == 1
    // 2 ** 1 == 2
    // 2 ** 2 == 4
    // 2 ** 3 == 8
    // 2 ** 4 == 16

    // pow(2, order + 1) - 1
    int addition = static_cast<int>(pow(2, order + 1) - 1);
    if (_is_right) {
        addition = -addition;
    }
    // if is_right -> to the left
    // else -> to the right
    return this + addition;
}

BuddyNode* BuddyNode::get_parent() {
    if (order == MAXIMUM_ORDER) {
        return nullptr;
    }
    // if is_right -> this - pow(2, order + 1)
    // else this - 1
    size_t offset = 1;
    if (is_right()) {
        offset = static_cast<size_t>(pow(2, order + 1));
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
    return this + static_cast<size_t>(pow(2, order));
}

// returns the buddy that's allocatable
BuddyNode* allocate_find(BuddyNode* node, BuddyAllocator* buddy, uint8_t _order) {
    // no point in traversing a branch when the current node is already taken
    if (node->is_taken()) {
        return nullptr;
    }

    if (_order == node->order && node->is_free()) {
        return node;
    }

    if (_order == node->order && !node->is_free()) {
        if (node->is_taken() || node->get_matching_buddy()->is_taken()) {
            return nullptr;
        }
        return node->get_matching_buddy();
    }

    auto left = allocate_find(node->get_left_child(), buddy, _order);
    if (left) {
        return left;
    }

    auto right = allocate_find(node->get_right_child(), buddy, _order);
    if (right) {
        return right;
    }

    return nullptr;  // we couldn't find an already existing node
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
        return root;
    }
    auto splittable_node = allocate_find(root, buddy, order);
    if (!splittable_node) {
        // panic??
        return nullptr;
    }
    // auto chunk = buddy->chunk_for(splittable_node->physical_addr);
    splittable_node = splittable_node->get_parent();
    auto temp = splittable_node;
    while (temp != root) {
        temp->split(buddy);
        temp = temp->get_parent();
    }
    temp->split(buddy);  // split the actual root node.
    return splittable_node;
}


size_t BuddyNode::pointed_size() const noexcept {
    return static_cast<size_t>(SMALLEST_CHUNK * pow(2, order));
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
    if (_is_split) {
        return;
    }
    _is_split = true;
    auto chunk = to_chunk(buddy);
    if (order == 0) {
        // panic; can't split a zero-order node
        return;
    }
    chunk->add_order(order, -1);
    chunk->add_order(order - 1, 2);
}

void BuddyNode::merge_children(BuddyAllocator* buddy, Chunk* matching_chunk) noexcept {
    auto left = get_left_child();
    auto right = get_right_child();

    if (!left->is_free() || !right->is_free()) {
        return;  // can't merge, one of the two children is taken
    }

    // can merge, let's.
    this->_is_split = false;
    if (!matching_chunk) {
        matching_chunk = to_chunk(buddy);
    }

    matching_chunk->add_order(order, 1);

    if (order > 0) {
        matching_chunk->add_order(static_cast<uint8_t>(order - 1), -2);
    }


    auto parent = get_parent();
    if (parent) {
        get_parent()->merge_children(buddy, matching_chunk);
    }
}

Chunk* BuddyNode::to_chunk(BuddyAllocator* buddy) const noexcept {
    return buddy->chunk_for(physical_addr);
}

BuddyAllocator::BuddyAllocator(void* base_address)
    : base_address{ reinterpret_cast<char*>(base_address) } {
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
    } while (order >= 0);

    // Now the state array
    return result;
}

size_t BuddyAllocator::estimate_memory_used(size_t address_range) {
    size_t trees_count = (address_range / LARGEST_CHUNK);
    size_t memory_for_buddy_nodes = calculate_nodes_for_max_order() * sizeof(BuddyNode);
    auto tree_size = memory_for_buddy_nodes + sizeof(Chunk);
    auto trees_size = trees_count * tree_size;
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
    size_t relative_address = reinterpret_cast<size_t>(address) - reinterpret_cast<size_t>(base_address);
    return chunk_at_index(relative_address / LARGEST_CHUNK);
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
        auto node = alloc<Chunk>();
        node->root = alloc<BuddyNode>();
        node->root->physical_addr = memory_base + (i * LARGEST_CHUNK);
        node->root->order = MAXIMUM_ORDER;
        node->free_values[MAXIMUM_ORDER] = 1;  // only an order MAX_ORDER node is available.
        auto left_child = alloc<BuddyNode>();
        left_child->order = MAXIMUM_ORDER - 1;
        left_child->physical_addr = memory_base + (i * LARGEST_CHUNK);
        create_tree_structure(left_child);
        auto right_child = alloc<BuddyNode>();
        right_child->order = MAXIMUM_ORDER - 1;
        right_child->physical_addr = reinterpret_cast<char*>(left_child->physical_addr) + (left_child->pointed_size() / 2);
        right_child->_is_right = true;
        create_tree_structure(right_child);

        assert(node->root->get_left_child() == left_child);
        assert(node->root->get_right_child() == right_child);
        assert(left_child->get_parent() == node->root);
        assert(right_child->get_parent() == node->root);
        assert(right_child->get_matching_buddy() == left_child);
        assert(left_child->get_matching_buddy() == right_child);
        assert(chunk_at_index(i) == node);
        assert(chunk_at_index(i)->free_values[MAXIMUM_ORDER] == 1);
    }

    buddy_heap.base = reinterpret_cast<BuddyInfoHeap*>(base_address + index);
    // finalize, aka construct the heap.
    for (size_t i = 0; i < zero_nodes_needed; i++) {
        buddy_heap.push(this, BuddyInfoHeap{
                                  chunk_at_index(i), static_cast<int8_t>(MAXIMUM_ORDER) });
        auto pushed_element = heap_index(i);
        assert(pushed_element->buddy == chunk_at_index(i));
        assert(pushed_element->largest_order_free == MAXIMUM_ORDER);
    }

    assert(buddy_heap.size() == zero_nodes_needed);
}

void BuddyAllocator::set_unusable_memory(char* start, char* end) noexcept {
    size_t start_base = start - static_cast<char*>(chunk_for(start)->root->physical_addr); 
    size_t count = (start - end) / LARGEST_CHUNK;
    for (size_t i = start_base; i < count; i++) {
        auto chunk = chunk_at_index(i);
        chunk->can_be_allocated = false;
        auto& values = chunk->free_values;
        firefly::std::fill(values.begin(), values.end(), -1);
        chunk->fix_heap(this);
    }
}

void BuddyAllocator::create_tree_structure(BuddyNode* parent_node) {
    uint8_t order = parent_node->order - 1;
    auto one = alloc<BuddyNode>();
    // Say parent_node covers physical addresses 0 through 16, then one would point to 0, two would point to 8
    // they would both have 50% of the memory of the parent.
    one->physical_addr = parent_node->physical_addr;
    one->order = order;
    if (order > 0) {
        create_tree_structure(one);
    }
    auto two = alloc<BuddyNode>();
    two->order = order;
    two->_is_right = true;
    // Since we know that MAXIMUM is divisibe by 2 (since it's a power of 2) we can just floor the address division.
    two->physical_addr = reinterpret_cast<char*>(parent_node->physical_addr) + (parent_node->pointed_size() / 2);
    if (order > 0) {
        create_tree_structure(two);
    }
    assert(one == parent_node->get_left_child());
    assert(one->get_parent() == parent_node);
    assert(two == parent_node->get_right_child());
    assert(two->get_parent() == parent_node);
    assert(one->get_matching_buddy() == two);
    assert(two->get_matching_buddy() == one);
}


void Chunk::add_order(size_t order, int count) {
    free_values[order] += count;
}

BuddyAllocator::allocation_result_t BuddyAllocator::allocate(uint8_t order) {
    auto& max = buddy_heap.max();
    auto state_before = max.buddy->free_values;
    if (!max.buddy->can_allocate(order)) {
        return { nullptr, 0 };
    }
    auto buddy = max.buddy->get_free_buddy(this, order);
    void* physical_addr = nullptr;
    if (order == MAXIMUM_ORDER) {
        buddy->_is_taken = true;
        physical_addr = buddy->physical_addr;
    } else {
        auto left = buddy->get_left_child();
        if (left->is_free()) {
            left->_is_taken = true;
            physical_addr = left->physical_addr;
        } else {
            auto right = buddy->get_right_child();
            right->_is_taken = true;
            physical_addr = right->physical_addr;
        }
    }
    max.buddy->add_order(order, -1);
    max.buddy->fix_heap(this);
    // std::cout << "Allocate order: " << int(order) << std::endl;
    // print(&buddy_heap);

    // for (size_t i = 0; i < max.buddy->free_values.max_size(); i++) {
    // 	std::cout << "Order: " << i << " free: " << (int)max.buddy->free_values[i] << std::endl;
    // }
    return { physical_addr, static_cast<uint8_t>(buddy->order == MAXIMUM_ORDER ? buddy->order : buddy->order - 1) };
}

BuddyNode* deallocate_find(void* addr, BuddyNode* buddy, uint8_t order) {
    if (buddy->physical_addr == addr && order == buddy->order) {
        return buddy;
    }

    if (buddy->get_left_child() == nullptr) {
        return nullptr;
    }

    auto one = deallocate_find(addr, buddy->get_left_child(), order);
    if (one) {
        return one;
    }

    auto two = deallocate_find(addr, buddy->get_right_child(), order);
    if (two) {
        return one;
    }

    return nullptr;
}

void BuddyAllocator::deallocate(void* addr, uint8_t order) {
    auto chunk = chunk_for(addr);
    // printBT(chunk->root);
    if (order == MAXIMUM_ORDER) {
        chunk->root->_is_taken = false;
        chunk->root->_is_split = false;
        chunk->free_values[MAXIMUM_ORDER] = 1;
    }

    auto allocated_buddy = deallocate_find(addr, chunk->root, order);
    if (!allocated_buddy) {
        return;
    }

    allocated_buddy->_is_taken = false;
    chunk->add_order(order, 1);
    allocated_buddy->get_parent()->merge_children(this, chunk);

    chunk->fix_heap(this);

    // std::cout << "Deallocate order: " << (int)order << std::endl;
    // printBT(chunk->root);

    // for (size_t i = 0; i < chunk->free_values.max_size(); i++) {
    // 	std::cout << "Order: " << i << " free: " << (int)chunk->free_values[i] << std::endl;
    // }
}

void Chunk::fix_heap(BuddyAllocator* buddy) {
    int8_t max_order = -1;
    int8_t max_value = -1;
    for (int8_t i = 0; static_cast<size_t>(i) < free_values.max_size(); i++) {
        if (free_values[i] >= max_value) {
            max_order = i;
            max_value = free_values[i];
        }
    }

    if (max_value == -1) {
        max_order = -1;
    }

    auto heap_element = buddy->heap_index(heap_index);
    auto before = heap_element->largest_order_free;
    if (max_order == before) {
        // we merged / split but the maximum available size didn't chagne
        return;
    }
    heap_element->largest_order_free = max_order;
    if (max_order > before) {
        heap_index = buddy->buddy_heap.heapify_up(heap_index);
    } else {
        heap_index = buddy->buddy_heap.heapify_down(heap_index);
    }
}

BuddyInfoHeap* BuddyAllocator::heap_index(size_t idx) {
    return buddy_heap.base + idx;
}

Chunk* BuddyAllocator::chunk_at_index(size_t index) {
    auto nodes_per_tree = calculate_nodes_for_max_order();
    auto bytes_for_nodes = sizeof(BuddyNode) * nodes_per_tree;
    auto bytes_for_chunk_headers = sizeof(Chunk);

    auto temp = (bytes_for_nodes * index) + (bytes_for_chunk_headers * index);
    return reinterpret_cast<Chunk*>(base_address + temp);
}
}  // namespace firefly::kernel::mm::buddy