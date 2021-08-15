#pragma once

#include "cstdlib/cstdint.h"
#include "cstdlib/cmath.h"

constexpr static inline size_t LARGEST_CHUNK = 2 * 1048576;
constexpr static inline size_t SMALLEST_CHUNK = 2 * 4096 * 4 * 4; // 64 kib
// casually casting a double to size_t because we _know_ that a power of 2 divided by a power of 2 that's larger
// will return an integer.
const static inline uint8_t MAXIMUM_ORDER = static_cast<uint8_t>(log2(LARGEST_CHUNK / SMALLEST_CHUNK));

struct Chunk;

// Do we want to store amount of memory that has been taken for the so-called zero nodes?
// probably not cause that memory doesn't have to be located next to eachother
struct BuddyNode {
    void* physical_addr; // this is always the same as split_one's physical address...

    BuddyNode* split_one;
    BuddyNode* split_two;
    
    BuddyNode* parent;

    uint8_t order : 6;
    uint8_t _is_taken : 1;
    uint8_t _is_split : 1;

    size_t pointed_size() const noexcept;

    bool is_split() const noexcept;
    bool is_taken() const noexcept;

    // if the taken_until pointer is null and this node isn't split
    // that means that this node hasn't yet been allocated from
    bool is_free() const noexcept;

    Chunk* to_chunk() const noexcept;
};

struct Chunk {
    BuddyNode root;
    uint8_t order_four_count : 1;
    uint8_t order_three_count : 1;
    uint8_t order_two_free : 1;
    uint8_t order_one_free : 1;
    uint8_t order_zero_free : 1;
};

struct BuddyInfoHeap {
    BuddyNode* buddy;
    uint8_t largest_order_free;

    bool operator<(BuddyInfoHeap const& rhs) const noexcept;
    bool operator>(BuddyInfoHeap const& rhs) const noexcept;
};


class BuddyAllocator {
public:
    Chunk* base_address;

    BuddyAllocator(void* base_address);

    static size_t calculate_nodes_for_max_order();
    static size_t estimate_memory_used(size_t address_range);

    Chunk* chunk_for(void* address);

    void initialize(size_t memory_available, char* memory_base);
    void create_tree_structure(BuddyNode* parent_node);

    BuddyNode* alloc_buddy_node();
    Chunk* alloc_chunk();
    BuddyInfoHeap* alloc_heap_node();

    void* allocate(size_t bytes);
    void deallocate(void* addr);
};

struct BuddyTreeHeap {
    BuddyInfoHeap* base;
    size_t _size = 0;

    size_t size();

    size_t parent(size_t index);

    size_t left(size_t index);

    size_t right(size_t index);

    void heapify_down(size_t i);

    void heapify_up(size_t i);

    BuddyInfoHeap* push(BuddyAllocator& buddy, BuddyInfoHeap key);

    BuddyInfoHeap& max();
};