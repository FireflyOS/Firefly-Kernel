#pragma once

#include <cstdlib/cassert.h>
#include <stddef.h>
#include <stdint.h>

#include <atomic>
#include <utility>

#include "firefly/compiler/clang++.hpp"
#include "firefly/memory-manager/mm.hpp"
#include "firefly/stivale2.hpp"
#include "libk++/bits.h"

static constexpr uint64_t GLOB_PAGE_ARRAY = AddressLayout::PageData + MiB(512);

enum class RawPageFlags : int {
    None = 0,
    Unusable = 1,
    Slab = 2
};

struct RawPage {
    RawPageFlags flags;
    int order;
    int buddy_index;
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
} ;

class Pagelist {
    using Index = int;
    using AddressType = uint64_t;

public:
    void init(stivale2_struct_tag_memmap *memmap_response) {
        for (size_t i = 0; i < memmap_response->entries; i++) {
            auto *e = &memmap_response->memmap[i];
            for (size_t j = 0; j <= e->length; j += 4096, largest_index++) {
                // clang-format off
                auto const page = RawPage
         		{
                    .flags = (e->type != STIVALE2_MMAP_USABLE) ? RawPageFlags::Unusable : RawPageFlags::None
                };
                pages[largest_index] = page;
                // clang-format on
            }
        }

        firefly::kernel::info_logger << firefly::kernel::info_logger.format("RawPage size: %d bytes\n", sizeof(RawPage));
        firefly::kernel::info_logger << firefly::kernel::info_logger.format("Pagelist overhead: %d Bytes\n", largest_index * sizeof(RawPage));

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

    RawPage *pages = (struct RawPage *)GLOB_PAGE_ARRAY;
    Index largest_index{};  // largest index into the 'pages' array
};

// Instance created in primary_phys.cpp
extern Pagelist pagelist;