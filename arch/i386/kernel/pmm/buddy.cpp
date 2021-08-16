#include "i386/pmm/buddy.hpp"

#include <cstdlib/cmath.h>
#include <utility.h>

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
	: base_address{ reinterpret_cast<Chunk*>(base_address) }
{}

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
	return calculate_nodes_for_max_order() * trees_count * sizeof(BuddyNode) +
		// Chunk that contains the counters too
		sizeof(Chunk);
}

// optional somehow
int8_t BuddyAllocator::order_for(size_t bytes) noexcept {
	if (bytes > LARGEST_CHUNK) {
		return -1;
	}
	int8_t result = 0; // default with an order 0 node, we start with smallest
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
 *
 * char* so we get mathematical operations on the pointer
 */
void BuddyAllocator::initialize(size_t memory_available, char* memory_base) {
	// NOTE: any memory that's exceeds a multiple of LARGEST_CHUNK will be LOST and not used.
	size_t zero_nodes_needed = memory_available / LARGEST_CHUNK;
	for (size_t i = 0; i < zero_nodes_needed; i++) {
		auto node = alloc_chunk();
		node->root.physical_addr = memory_base + (i * LARGEST_CHUNK);
		node->root.order = MAXIMUM_ORDER;
		node->order_four_count = 1; // only an order 4 node is available.
		node->root.split_one = alloc_buddy_node();
		node->root.split_two = alloc_buddy_node();
		node->root.split_one->order = 1;
		node->root.split_two->order = 1;
		node->root.split_one->parent = &node->root;
		node->root.split_two->parent = &node->root;
		
		// zero-node has been created, create all the child nodes that we need.
		create_tree_structure(node->root.split_one);
		create_tree_structure(node->root.split_two);
	}

	// finalize, aka construct the heap.
}

void BuddyAllocator::create_tree_structure(BuddyNode* parent_node) {
	uint8_t order = parent_node->order - 1;
	auto one = alloc_buddy_node();
	auto two = alloc_buddy_node();
	one->order = order;
	two->order = order;
	one->parent = parent_node;
	two->parent = parent_node;

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
	*temp = Chunk{}; // default construct it so all fields are 0
	return temp;
}
	
BuddyInfoHeap* BuddyAllocator::alloc_heap_node() {
	auto temp = reinterpret_cast<BuddyInfoHeap*>(base_address);
	*temp = BuddyInfoHeap{};
	(*reinterpret_cast<BuddyInfoHeap**>(&base_address))++;
	return temp;
}

void* BuddyAllocator::allocate(size_t bytes) { return (void*)bytes; }
void BuddyAllocator::deallocate(void* addr) {(void)addr;}



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
		heapify_down(largest);
	}
}

void BuddyTreeHeap::heapify_up(size_t i) {
	auto parent = this->parent(i);
	if (i && base[parent] < base[i]) {
		firefly::std::swap(base[i], base[parent]);
		heapify_up(parent);
	}
}

BuddyInfoHeap* BuddyTreeHeap::push(BuddyAllocator& buddy, BuddyInfoHeap key) {
	auto node = buddy.alloc_heap_node();
	*node = key;
	size_t index = size() - 1;
	heapify_up(index);
	return node;
}

BuddyInfoHeap& BuddyTreeHeap::max() {
	return base[0];
}