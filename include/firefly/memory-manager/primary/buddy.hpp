#pragma once

#include <cstdlib/cassert.h>  // assert_truth
#include <cstdlib/cstring.h>  // memset
#include <stdint.h>

#include <algorithm>  // std::min, std::max

#include "firefly/logger.hpp"

namespace firefly::kernel::mm {
template <int highest_order>
class BuddyAllocator {
public:
    enum FillMode : char {
        ZERO = 0,
        NONE = 1  // Don't fill
    };

    using Order = int;
    using PhysicalAddress = uint64_t *;

    constexpr static Order min_order = 9;
    constexpr static Order max_order = highest_order - 3;
    constexpr static bool verbose = false;

    BuddyAllocator(PhysicalAddress base)
        : base(base) {
        if constexpr (verbose)
            info_logger << "min-order: " << min_order << ", max-order: " << max_order << logger::endl;

        freelist.add(base, max_order - min_order);
    }

    PhysicalAddress alloc_order(Order order, FillMode fill = FillMode::NONE) {
        return alloc(1 << order, fill);
    }

    PhysicalAddress alloc(uint64_t size, [[maybe_unused]] FillMode fill = FillMode::NONE) {
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

        return block;
    }

    void free(PhysicalAddress block, Order order) {
        if (block == nullptr)
            return;

        order -= 3;

        // There are no buddies at max_order
        if (order == max_order) {
            freelist.add(block, max_order - min_order);
            return;
        }

        coalesce(block, order);
    }

private:
    template <typename T, int orders>
    class Freelist {
    private:
        T list[orders + 1]{};  // Todo: Think of a way to allocate enough memory for this array dynamically

    public:
        void add(const T &block, Order order) {
            if (block)
                *(T *)block = list[order];

            list[order] = block;
        }

        T remove(Order order) {
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

    static constexpr int log2(int size) {
        int result = 0;
        while ((1 << result) < size) {
            ++result;
        }
        return result;
    }

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
    Freelist<PhysicalAddress, max_order - min_order> freelist;
    PhysicalAddress base;
};
}  // namespace firefly::kernel::mm