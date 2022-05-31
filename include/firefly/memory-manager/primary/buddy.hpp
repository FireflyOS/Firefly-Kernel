#pragma once

#include <cstdlib/cassert.h>  // assert_truth
#include <cstdlib/cstring.h>  // memset
#include <stdint.h>

#include <algorithm>  // std::min, std::max

#include "firefly/logger.hpp"
#include "firefly/memory-manager/mm.hpp"

namespace firefly::kernel::mm {
class BuddyAllocator {
public:
    using Order = int;
    using PhysicalAddress = uint64_t *;

    Order max_order = 0;                                // Represents the largest allocation and is determined at runtime.
    constexpr static Order largest_allowed_order = 30;  // 1GiB is the largest allocation on instance of this class may serve.
    constexpr static Order min_order = 9;               // 4kib, this is the smallest allocation size and will never change.
    constexpr static bool verbose{}, sanity_checks{};   // sanity_checks ensures we don't go out-of-bounds on the freelist.
                                                        // Beware: These options will impact the performance of the allocator.

    BuddyAllocator(PhysicalAddress base, int target_order)
        : max_order(target_order - 3), base(base) {
        if constexpr (verbose)
            info_logger << "min-order: " << min_order << ", max-order: " << max_order << logger::endl;

        freelist.add(base, max_order - min_order);
    }

    // When a constructor is not used it's most likely used as part of a 'struct Zone'.
    // Therefore we allow the kernel to add information to the allocator later via 'init_from_zone'
    void init_from_zone(PhysicalAddress base, int target_order) {
        // Constructor has been called already..
        if (this->base)
            return;

        this->base = base;
        max_order = target_order - 3;

        if constexpr (verbose)
            info_logger << "min-order: " << min_order << ", max-order: " << max_order << logger::endl;

        freelist.add(base, max_order - min_order);
    }

    PhysicalAddress alloc_order(Order order, FillMode fill = FillMode::ZERO) {
        return alloc(1 << order, fill);
    }

    PhysicalAddress alloc(uint64_t size, FillMode fill = FillMode::ZERO) {
        Order order = std::max(min_order, log2(size >> 3));
        assert_truth(order <= max_order);

        if constexpr (verbose)
            info_logger << "Suitable order for allocation of size '" << size << "' is: " << order << logger::endl;

        PhysicalAddress block = nullptr;
        Order ord = order;

        for (; ord <= max_order; ord++) {
            block = freelist.remove(ord - min_order);
            if (block != nullptr)
                break;
        }

        if (block == nullptr) {
            if constexpr (verbose)
                info_logger << "Block is a nullptr (order: " << order << ", size: " << size << ")\n";

            return nullptr;
        }

        // Split higher order blocks
        while (ord-- > order) {
            auto buddy = buddy_of(block, ord);
            freelist.add(buddy, ord - min_order);
        }

        if (fill != FillMode::NONE)
            memset(static_cast<void *>(block), fill, size);

        // info_logger << "Allocated " << block << " at order " << ord << " (max: " << max_order << ")  with a size of " << size << '\n';
        return block;
    }

    void free(PhysicalAddress block, Order order) {
        order -= 3;

        if (block == nullptr || order > max_order || order < min_order)
            return;

        if (!freelist.get_element(order - min_order) && order != max_order)
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
        T list[orders + 1]{};

    public:
        void add(const T &block, Order order) {
            if constexpr (sanity_checks)
                if (order < min_order || order > largest_allowed_order)
                    assert_truth(!"Order mismatch");

            if (block)
                *(T *)block = list[order];

            list[order] = block;
        }

        T remove(Order order) {
            if constexpr (sanity_checks)
                if (order < min_order || order > largest_allowed_order)
                    assert_truth(!"Order mismatch");

            T element = list[order];

            // info_logger << "element: " << info_logger.hex(element) << " order: " << order << "\n";

            if (element == nullptr)
                return nullptr;

            list[order] = *(T *)list[order];

            return element;
        }

        T get_element(Order order) const {
            return list[order];
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

    inline PhysicalAddress buddy_of(PhysicalAddress block, Order order) {
        return base + ((block - base) ^ (1 << order));
    }

    void coalesce(PhysicalAddress block, Order order) {
        // Description:
        // Try to merge 'block' and it's buddy into one larger block at 'order + 1'
        // If both block are free, remove them and insert the smaller of
        // the two blocks into the next highest order and repeat that process.
        if (order == max_order)
            return;

        PhysicalAddress buddy = buddy_of(block, order);
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
    Freelist<PhysicalAddress, largest_allowed_order - min_order> freelist;
    PhysicalAddress base{};
};
}  // namespace firefly::kernel::mm