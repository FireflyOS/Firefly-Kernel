#pragma once

#include <stdint.h>
#include <cstdlib/cstring.h>

#include "firefly/logger.hpp"
#include "firefly/memory-manager/mm.hpp"
#include "firefly/stivale2.hpp"
#include "firefly/trace/strace.hpp"

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
                info_logger << "freelist::add(): Added '" << info_logger.hex(block) << "' to the freelist\n";
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
                info_logger << info_logger.format("base: 0x%x | top: 0x%x | pages: %d\n", base, top, pages);

            if (!pages)
                continue;

            for (auto i = 0ul; i <= pages; i++) {
                freelist.add(reinterpret_cast<PhysicalAddress>(base + (i * PAGE_SIZE)));
            }
        }
        info_logger << "pmm: Initialized " << logger::endl;
    }

    static PhysicalAddress allocate(FillMode fill = FillMode::ZERO) {
        return freelist.remove(fill);
    }

    static void deallocate(PhysicalAddress ptr) {
        freelist.add(ptr);
    }
};
}  // namespace firefly::kernel::mm