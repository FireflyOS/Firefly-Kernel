#pragma once

#include "firefly/logger.hpp"
#include "firefly/memory-manager/mm.hpp"
#include "firefly/memory-manager/primary/primary_phys.hpp"
#include "libk++/bits.h"
#include "libk++/memory.hpp"

namespace firefly::kernel::mm {
class PageFrame {
public:
    static constexpr bool verbose = !false;

    template <typename T>
    class Freelist {
    private:
        T list;

    public:
        void add(const T &block) {
            *(T *)block = list;
            list = block;

            // if constexpr (verbose)
            //     info_logger << "freelist::add(): Added '" << info_logger.hex(block) << "' to the freelist\n";
        }

        T remove(FillMode fill) {
            T element = list;
            list = *(T *)list;

            if (fill != FillMode::NONE)
                memset(element, fill, PAGE_SIZE);

            return element;
        }
    };
    Freelist<PhysicalAddress> freelist;

    void init(PhysicalAddress base, size_t length) {
        freelist.add(nullptr);
        addRange(reinterpret_cast<uint64_t>(base), length);
        info_logger << "page-allocator: Initialized " << logger::endl;
    }

    PhysicalAddress allocate(FillMode fill = FillMode::ZERO) {
        auto ptr = freelist.remove(fill);
        if (!ptr) {
            auto fallback = Physical::allocate(4 * PAGE_SIZE);
            if (!fallback)
                return nullptr;

            addRange(reinterpret_cast<uint64_t>(fallback), 4 * PAGE_SIZE);
            return freelist.remove(FillMode::NONE);  // Physical::Allocate already cleared the memory
        }
        return ptr;
    }

    void deallocate(PhysicalAddress ptr) {
        if (ptr)
            freelist.add(ptr);
    };

    void addRange(uint64_t base, uint64_t len) {
        auto pages = ((base + len - 1) - base) / PAGE_SIZE;
        auto top = base + pages * PAGE_SIZE;

        if constexpr (verbose)
            info_logger << info_logger.format("base: 0x%x | top: 0x%x | pages: %d\n", base, top, pages);

        for (auto i = 0ul; i < (len / PAGE_SIZE); i++) {
            for (auto i = 0ul; i <= pages; i++) {
                freelist.add(reinterpret_cast<PhysicalAddress>(base + (i * PAGE_SIZE)));
            }
        }
    }
};
}  // namespace firefly::kernel::mm
