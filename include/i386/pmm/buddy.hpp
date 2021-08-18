#pragma once

#include "cstdlib/cmath.h"
#include "cstdlib/cstdint.h"
#include "array.h"

constexpr inline size_t constexpr_log2(size_t n) {
    return ((n<2) ? 1 : 1 + constexpr_log2(n / 2));
}

constexpr static inline size_t SMALLEST_CHUNK = 4096UL;  // 1 page
constexpr static inline size_t LARGEST_CHUNK = SMALLEST_CHUNK * 16UL; // 16 pages

// casually casting a double to size_t because we _know_ that a power of 2 divided by a power of 2 that's larger
// will return an integer.
constexpr static inline uint8_t MAXIMUM_ORDER = static_cast<uint8_t>(constexpr_log2(LARGEST_CHUNK / SMALLEST_CHUNK));

struct Chunk;
class BuddyAllocator;

// Do we want to store amount of memory that has been taken for the so-called zero nodes?
// probably not cause that memory doesn't have to be located next to eachother
struct BuddyNode {
    void* physical_addr;  // this is always the same as split_one's physical address...

    uint8_t order : 5;
    uint8_t _is_taken : 1;
    uint8_t _is_split : 1;
    uint8_t _is_right : 1;

    size_t pointed_size() const noexcept;

    bool is_split() const noexcept;
    bool is_taken() const noexcept;
    bool is_right() const noexcept;

    BuddyNode* get_left_child();
    BuddyNode* get_right_child();
    BuddyNode* get_parent();
    BuddyNode* get_matching_buddy();

    // if the taken_until pointer is null and this node isn't split
    // that means that this node hasn't yet been allocated from
    bool is_free() const noexcept;

    Chunk* to_chunk(BuddyAllocator* buddy) const noexcept;

    void split(BuddyAllocator* buddy) noexcept;
    void merge_buddy(BuddyAllocator* buddy) noexcept;
};

struct Chunk {
    BuddyNode root;
    size_t heap_index;
    uint8_t can_be_allocated : 1;
    firefly::std::array<uint8_t, MAXIMUM_ORDER> free_values;

    bool can_allocate(uint8_t order) const noexcept;
    BuddyNode* get_free_buddy(BuddyAllocator* buddy, uint8_t order) noexcept;
    void fix_heap(BuddyAllocator* buddy);
};

struct BuddyInfoHeap {
    Chunk* buddy;
    int8_t largest_order_free;

    bool operator<(BuddyInfoHeap const& rhs) const noexcept;
    bool operator>(BuddyInfoHeap const& rhs) const noexcept;
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

    BuddyInfoHeap* push(BuddyAllocator* buddy, BuddyInfoHeap key);

    BuddyInfoHeap& max();
};

class BuddyAllocator {
public:
    Chunk* base_address;
    BuddyTreeHeap buddy_heap;

    BuddyAllocator(void* base_address);

    static size_t calculate_nodes_for_max_order();
    static size_t estimate_memory_used(size_t address_range);
    
    static int8_t order_for(size_t bytes) noexcept;
    Chunk* chunk_for(void* address);

    void initialize(size_t memory_available, char* memory_base);
    void create_tree_structure(BuddyNode* parent_node);

    BuddyNode* alloc_buddy_node();
    Chunk* alloc_chunk();
    BuddyInfoHeap* alloc_heap_node();

    void* allocate(uint8_t order);
    void deallocate(void* addr, uint8_t order);

    BuddyInfoHeap* heap_index(size_t idx);
};

