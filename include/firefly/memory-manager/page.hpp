#pragma once

#include <cstdlib/cassert.h>
#include <stddef.h>
#include <stdint.h>

#include <atomic>
#include <utility>

#include "firefly/compiler/compiler.hpp"
#include "firefly/limine.hpp"
#include "firefly/memory-manager/mm.hpp"
#include "libk++/bits.h"
extern uintptr_t GLOB_PAGE_ARRAY[];

enum class RawPageFlags : int {
    None = 0,
    Unusable = 1,
    Slab = 2
};

struct RawPage {
    RawPageFlags flags;
    int order;
    union {
        int buddy_index;
        int slab_size;
    };
    std::atomic_int refcount;

    void operator=(const RawPage &other) {
        flags = other.flags;
        order = other.order;
        buddy_index = other.buddy_index;
        refcount.store(other.refcount, std::memory_order_seq_cst);
    }

    bool is_buddy_page(int min_order) const {
        return order >= min_order;
    }

    void reset(bool reset_refcount = true) {
        flags = RawPageFlags::None;
        order = 0;
        if (likely(reset_refcount))
            refcount = 0;
    }
};

class Pagelist {
    using Index = int;
    using AddressType = uint64_t;

public:
    void init(struct limine_memmap_response *memmap_response) {
        for (size_t i = 0; i < memmap_response->entry_count; i++) {
            auto e = memmap_response->entries[i];

            // Ensure the array size is not exceeded
            // auto const projected_index = largest_index + (e->length / 4096);
            // assert_truth(projected_index <= page_array_sz);

            for (size_t j = 0; j <= e->length; j += 4096, largest_index++) {
                auto const page = RawPage{
                    .flags = (e->type != LIMINE_MEMMAP_USABLE) ? RawPageFlags::Unusable : RawPageFlags::None
                };
                pages[largest_index] = page;
            }
        }

        firefly::kernel::ConsoleLogger::log() << firefly::kernel::ConsoleLogger::log().format("RawPage size: %d bytes\n", sizeof(RawPage));
        firefly::kernel::ConsoleLogger::log() << firefly::kernel::ConsoleLogger::log().format("Pagelist overhead: %d Bytes\n", largest_index * sizeof(RawPage));

        // largest_index is incremented one too many times.
        --largest_index;
    }

    inline auto get_page(RawPage *p) const {
        return AddressType(pages - p);
    }
    inline auto max_page() const {
        return &pages[largest_index];
    }
    inline auto min_page() const {
        return &pages[0];
    }

    inline auto phys_to_page(uint64_t addr) const {
        return &pages[(addr >> PAGE_SHIFT) - 1];
    }

    inline auto page_to_phys(RawPage *p) const {
        return get_page(p) << PAGE_SHIFT;
    }

    auto operator[](Index i) const {
        auto const &page = phys_to_page(i);
        return array_operator{ .page = page, .address = page_to_phys(page) };
    }

private:
    struct array_operator {
        RawPage *page;
        AddressType address;
    };

    RawPage *pages = (struct RawPage *)(reinterpret_cast<uintptr_t>(GLOB_PAGE_ARRAY));
    Index largest_index{};  // largest index into the 'pages' array
};

// Instance created in primary_phys.cpp
extern Pagelist pagelist;