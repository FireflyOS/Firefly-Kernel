#pragma once

#include "firefly/logger.hpp"
#include "firefly/memory-manager/mm.hpp"

namespace firefly::kernel::mm {
class PageFrame {
public:
    static constexpr bool verbose = false;

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
    Freelist<PhysicalAddress> freelist;

    void init(PhysicalAddress base, size_t length) {
        freelist.add(nullptr);

        auto base = base;
        auto len = length;
        auto pages = ((base + len - 1) - base) / PAGE_SIZE;
        auto top = base + pages * PAGE_SIZE;

        for (auto i = 0ul; i < (length / PAGE_SIZE); i++) {
            if constexpr (verbose)
                info_logger << info_logger.format("base: 0x%x | top: 0x%x | pages: %d\n", base, top, pages);

            for (auto i = 0ul; i <= pages; i++) {
                freelist.add(reinterpret_cast<PhysicalAddress>(base + (i * PAGE_SIZE)));
            }
        }
        info_logger << "page-allocator: Initialized " << logger::endl;
    }

    PhysicalAddress allocate(FillMode fill = FillMode::ZERO) {
        return freelist.remove(fill);
    }

    void deallocate(PhysicalAddress ptr) {
        if (ptr)
            freelist.add(ptr);
    };
}  // namespace firefly::kernel::mm
