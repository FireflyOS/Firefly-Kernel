#pragma once

#include <stdint.h>
#include <stl/cstdlib/cstring.h>
#include <stl/cstdlib/stdio.h>

#include "x86_64/memory-manager/mm.hpp"
#include "x86_64/stivale2.hpp"
#include "x86_64/trace/strace.hpp"

namespace firefly::kernel::mm {

namespace {
constexpr bool verbose = false;
}

class pmm {
public:
    using PhysicalAddress = void *;

    enum FillMode : char {
        ZERO = 0,
        NONE = 1  // Don't fill
        // Todo: Asan fill mode?
    };

    template <typename T>
    class Freelist {
    private:
        T list;

    public:
        void add(const T &block) {
            *(T *)block = list;
            list = block;

            if constexpr (verbose)
                printf("freelist::add(): Added '0x%x' to the freelist\n", block);
        }

        T remove(FillMode fill) {
            T element = list;
            list = *(T *)list;

            if (fill != FillMode::NONE)
                memset(element, fill, PAGE_SIZE);

            return element;
        }
    };
    static Freelist<PhysicalAddress> freelist;

    static void init(stivale2_struct_tag_memmap *mmap) {
        freelist.add(nullptr);

        for (auto i = 0ul; i < mmap->entries; i++) {
            auto entry = mmap->memmap[i];
            if (entry.type != STIVALE2_MMAP_USABLE)
                continue;

            auto base = entry.base;
            auto len = entry.length;
            auto pages = ((base + len - 1) - base) / PAGE_SIZE;
            auto top = base + pages * PAGE_SIZE;

            if constexpr (verbose)
                printf("base: 0x%x | top: 0x%x | pages: %d\n", base, top, pages);

            if (!pages)
                continue;

            for (auto i = 0ul; i <= pages; i++) {
                freelist.add(reinterpret_cast<PhysicalAddress>(base + (i * PAGE_SIZE)));
            }
        }
        printf("pmm: Initialized\n");
    }

    static PhysicalAddress allocate(FillMode fill = FillMode::ZERO) {
        return freelist.remove(fill);
    }

    static void deallocate(PhysicalAddress ptr) {
        freelist.add(ptr);
    }
};
}  // namespace firefly::kernel::mm