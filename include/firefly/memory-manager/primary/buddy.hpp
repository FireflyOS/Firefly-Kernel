#pragma once

#include <stdint.h>

#include <algorithm>

#include "cstdlib/cmath.h"
#include "firefly/stivale2.hpp"
// #include "memory.hpp"
#include <utility>

#include "cstdlib/cassert.h"
#include "cstdlib/cstring.h"
#include "firefly/logger.hpp"
#include "firefly/memory-manager/page.hpp"
#include "libk++/align.h"


struct BuddyAllocationResult {
private:
    using Order = int;
    using AddressType = uint64_t *;

public:
    AddressType ptr{ nullptr };
    Order order{ 0 };
    int npages;

public:
    BuddyAllocationResult() = default;
    BuddyAllocationResult(const AddressType &block, Order ord, int num_pages)
        : ptr(block), order(ord), npages(num_pages) {
    }
    inline AddressType unpack() const {
        return ptr;
    }
};

class BuddyAllocator {
public:
    using Order = int;
    using AddressType = uint64_t *;

    Order max_order = 0;                                // Represents the largest allocation and is determined at runtime.
    constexpr static Order min_order = 9;               // 4kib, this is the smallest allocation size and will never change.
    constexpr static Order largest_allowed_order = 30;  // 1GiB is the largest allocation an instance of this class may serve.
    constexpr static bool verbose{}, sanity_checks{};   // sanity_checks ensures we don't go out-of-bounds on the freelist.
                                                        // Beware: These options will impact the performance of the allocator.

    void init(AddressType base, int target_order) {
        this->base = base;
        max_order = target_order - 3;

        if constexpr (verbose)
            firefly::kernel::info_logger << firefly::kernel::info_logger.format("min-order: %d, max-order: %d", min_order, max_order);

        freelist.init();
        freelist.add(base, max_order - min_order);
    }

    auto alloc(uint64_t size, FillMode fill = FillMode::ZERO) {
        Order order = std::max(min_order, log2(size >> 3));

        if constexpr (sanity_checks) {
            if (order > max_order) {
                if constexpr (verbose)
                    firefly::kernel::info_logger << firefly::kernel::info_logger.format("Requested order %d (%d) is too large for this buddy instance | max-order is: %d", order, size, max_order);

                return BuddyAllocationResult();
            }
        }

        if constexpr (verbose)
            firefly::kernel::info_logger << firefly::kernel::info_logger.format("Suitable order for allocation of size '%d' is: %d", size, order);

        AddressType block = nullptr;
        Order ord = order;

        for (; ord <= max_order; ord++) {
            block = freelist.remove(ord - min_order);
            if (block != nullptr)
                break;
        }

        if (block == nullptr) {
            if constexpr (verbose)
                firefly::kernel::info_logger << firefly::kernel::info_logger.format("Block is a nullptr (order: %d, size: %d)", order, size);

            return BuddyAllocationResult();
        }

        // Split higher order blocks
        while (ord-- > order) {
            auto buddy = buddy_of(block, ord);
            freelist.add(buddy, ord - min_order);
        }

        // 'size' is not guaranteed to be a power of two. (Hence the manual pow2)
        const auto correct_size = (1 << (order + 3));

        if (fill != FillMode::NONE)
            memset(static_cast<void *>(block), fill, correct_size);

        if constexpr (verbose)
            firefly::kernel::info_logger << firefly::kernel::info_logger.format("Allocated 0xlx at order %d (max: %d | min: %d) with a size of %d", block, ord, max_order, min_order, size);

        return BuddyAllocationResult(block, ord + 1, correct_size / PAGE_SIZE);
    }

    void free(AddressType block, Order order) {
        assert_truth(order >= min_order && order <= max_order && "Invalid order passed to free()");
        if (block == nullptr)
            return;

        // There are no buddies at max_order
        if (order == max_order) {
            freelist.add(block, max_order - min_order);
            return;
        }

        coalesce(block, order);
    }

    int log2(int size) {
        int result = 0;
        while ((1 << result) < size) {
            ++result;
        }
        return result;
    }

private:
    template <typename T, int orders>
    class Freelist {
    private:
        T list[orders + 1]{ nullptr };

    public:
        void init() {
            for (int i = 0; i < orders + 1; i++)
                list[i] = nullptr;
        }

        void add(const T &block, Order order) {
            if constexpr (sanity_checks)
                if (order < min_order || order > largest_allowed_order)
                    assert_truth(!"Order mismatch");

            if (block) {
                memset((void *)block, 0, PAGE_SIZE);
                *(T *)block = list[order];
            }
            list[order] = block;
        }

        T remove(Order order) {
            if constexpr (sanity_checks)
                if (order < min_order || order > largest_allowed_order)
                    assert_truth(!"Order mismatch");

            T element = list[order];

            if (element == nullptr)
                return nullptr;

            list[order] = *(T *)list[order];
            return element;
        }

        T next(const T &block) const {
            return *reinterpret_cast<T *>(block);
        }

        bool find(const T &block, Order order) const {
            T element = list[order];
            while (element != nullptr && element != block) {
                element = next(element);
            }

            return element != nullptr;
        }
    };

    inline AddressType buddy_of(AddressType block, Order order) {
        return base + ((block - base) ^ (1 << order));
    }

    void coalesce(AddressType block, Order order) {
        // Description:
        // Try to merge 'block' and it's buddy into one larger block at 'order + 1'
        // If both block are free, remove them and insert the smaller of
        // the two blocks into the next highest order and repeat that process.
        if (order == max_order)
            return;

        AddressType buddy = buddy_of(block, order);
        if (buddy == nullptr)
            return;

        // Scan freelist for the buddy. (This isn't really efficient)
        // Could be optimized with a bitmap to track the state of each node.
        // The overhead of a bitmap would be minimal + the allocator itself has little to no overhead so size complexity should not be of concern.
        bool is_buddy_free = freelist.find(buddy, order - min_order);

        // Buddy block is free, merge them together
        if (is_buddy_free) {
            freelist.remove(order);
            coalesce(std::min(block, buddy), order + 1);  // std::min ensures that the smaller block of memory is merged with a larger and not vice-versa (which wouldn't work)
        }

        // The buddy is not free and merging is not possible.
        // Therefore it is simply put back onto the freelist.
        freelist.add(block, order - min_order);
    }

private:
    Freelist<AddressType, largest_allowed_order - min_order> freelist;
    AddressType base{};
};

class BuddyManager {
    using AddressType = BuddyAllocator::AddressType;
    using Index = uint64_t;

public:
    void init(struct stivale2_struct_tag_memmap *memmap_response) {
        highest_address = memmap_response->memmap[memmap_response->entries - 1].base + memmap_response->memmap[memmap_response->entries - 1].length;

        sort(std::move(memmap_response));
        auto num_buddies = reserve_buddy_allocator_memory(std::move(memmap_response));
        memset(static_cast<void *>(buddies), 0, sizeof(BuddyAllocator) * num_buddies);

        Index idx{};
        uint64_t total{};
        for (Index i = 0; i < memmap_response->entries; i++) {
            auto *e = &memmap_response->memmap[i];
            if (e->type != STIVALE2_MMAP_USABLE || e->length <= PAGE_SIZE)
                continue;

            for (uint64_t j = 0; j < 64; j++) {
                if (e->length & (1ll << j)) {
                    auto top = e->base + (1ll << j);
                    auto size_bytes = top - e->base;
                    buddies[idx++].init((uint64_t *)e->base, log2(size_bytes));
                    e->base += (1ll << j);
                    total += (1ll << j);
                }
            }
        }
        top_idx = idx - 1;

        // Iterate over elements in the base array
        for (Index i = 0; i < idx; i++) {
            // Sort the elements by swapping them where applicable.
            [&]() {
                auto &original = buddies[i];

                for (Index j = i + 1; j < idx; j++) {
                    auto &current = buddies[j];

                    if (current.max_order > original.max_order)
                        std::swap(current, original);
                }
            }();
        }

        firefly::kernel::info_logger << firefly::kernel::info_logger.format("Managing a grand total of: %d bytes\n", total);
    }

    // Returns the highest address in the memory map.
    // This does NOT mean it is usable memory!
    uint64_t get_highest_address() const {
        return highest_address;
    }

    AddressType must_alloc(uint64_t size, FillMode fill = FillMode::NONE) {
        auto ptr = this->alloc(size, fill);
        if (!ptr)
            firefly::panic("must_alloc failed to allocate memory!");

        return ptr;
    }

    AddressType alloc(uint64_t size, FillMode fill = FillMode::NONE) {
        BuddyAllocator::Order order = log2(size);
        Index min_idx = suitable_buddy(order);

        // Buddy allocators are sorted from the largest to smallest order
        for (Index i = min_idx; i > 0; i--) {
            // Perform the allocation
            auto ptr = buddies[i].alloc(size, fill);

            if (ptr.unpack()) {
                // Mark the allocated pages as such in the pagelist
                auto npages = ptr.npages;
                auto base = reinterpret_cast<uint64_t>(ptr.unpack());

                for (int j = 0; j < npages; j++, base += PAGE_SIZE) {
                    auto page = pagelist.phys_to_page(base);
                    page->refcount++;
                    page->order = ptr.order;
                    page->buddy_index = i;
                }

                return ptr.unpack();
            }
        }

        return nullptr;
    }

    void free(AddressType ptr) {
        auto page = pagelist.phys_to_page(reinterpret_cast<uint64_t>(ptr));

        // Not a buddy page
        if (!page->is_buddy_page(BuddyAllocator::min_order))
            return;

        // Save some data before the page gets reset.
        int buddy_index = page->buddy_index;
        int order = page->order;

        // Mark the pages in the pagelist as free and perform some checks
        uint32_t npages = (1 << (page->order + 3)) / PAGE_SIZE;
        auto base = reinterpret_cast<uint64_t>(ptr);

        for (uint32_t i = 0; i < npages; i++, base += PAGE_SIZE) {
            page = pagelist.phys_to_page(base);
            assert_truth(page->refcount == 1 && "This pages refcount is not 1. This means that there was an attempt to free an actively used block of memory");
            page->reset();
        }

        buddies[buddy_index].free(ptr, order);
    }

private:
    // Selection sort
    inline void sort(stivale2_struct_tag_memmap *mmap) {
        // Iterate over elements in the base array
        for (uint64_t i = 0; i < mmap->entries; i++) {
            // Sort the elements by swapping them where applicable.
            [&]() {
                auto *original = &mmap->memmap[i];

                for (uint64_t j = i + 1; j < mmap->entries; j++) {
                    auto *current = &mmap->memmap[j];
                    if (current->length > original->length)
                        std::swap(current, original);
                }
            }();
        }
    }

    inline uint64_t buddies_required(stivale2_struct_tag_memmap *mmap) {
        uint64_t num_buddies_required = 0;
        for (Index i = 0; i < mmap->entries; i++) {
            const auto *e = &mmap->memmap[i];
            if (e->type != STIVALE2_MMAP_USABLE || e->length <= PAGE_SIZE)
                continue;

            for (uint64_t j = 0; j < 64; j++)
                if (e->length & (1LL << j))
                    ++num_buddies_required;
        }

        assert_truth(num_buddies_required > 0ul && "Bad memory map?");
        return num_buddies_required;
    }

    inline uint64_t reserve_buddy_allocator_memory(stivale2_struct_tag_memmap *mmap) {
        const auto size = buddies_required(std::move(mmap)) * sizeof(BuddyAllocator);

        for (Index i = 0; i < mmap->entries; i++) {
            auto *e = &mmap->memmap[i];
            if (e->type != STIVALE2_MMAP_USABLE || e->length < size)
                continue;

            firefly::kernel::info_logger << firefly::kernel::info_logger.format("Creating %d large hole at region [0x%x-0x%x]\n", size, e->base, e->base + e->length);
            buddies = reinterpret_cast<BuddyAllocator *>(e->base);

            e->base = firefly::libkern::align_up4k(e->base + size);
            e->length -= firefly::libkern::align_down4k(size);
            return (size / sizeof(BuddyAllocator));
        }

        assert_truth(!"Failed to reserve memory for the buddy allocators!");
        __builtin_unreachable();
    }

    inline Index suitable_buddy(uint64_t suitable_order) {
        Index suitable_index{};
        for (Index i = 0; i < top_idx; i++) {
            [&]() {
                auto &original = buddies[i];

                for (Index j = i + 1; j < top_idx; j++) {
                    auto &current = buddies[j];

                    if (current.max_order < original.max_order && current.max_order >= static_cast<BuddyAllocator::Order>(suitable_order))
                        suitable_index = j;
                }
            }();
        }
        return suitable_index;
    }

private:
    uint64_t highest_address;
    BuddyAllocator *buddies;
    Index top_idx{};
};

// Instance created in primary_phys.cpp
extern BuddyManager buddy;