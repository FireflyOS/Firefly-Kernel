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
            if (block) {
                memset((void *)block, 0, PageSize::Size4K);
                *(T *)block = list;
            }
            list = block;
        }

        T remove(FillMode fill) {
            T element = list;
            list = *(T *)list;

            if (fill != FillMode::NONE)
                memset(element, fill, PageSize::Size4K);

            return element;
        }
    };
    Freelist<PhysicalAddress> freelist;

    void init(PhysicalAddress base, size_t length) {
        freelist.add(nullptr);
        addRange(reinterpret_cast<uint64_t>(base), length);
        ConsoleLogger::log() << "page-allocator: Initialized " << logger::endl;
    }

    PhysicalAddress allocate(FillMode fill = FillMode::ZERO) {
        auto ptr = freelist.remove(fill);
        if (!ptr) {
            auto fallback = Physical::allocate(4 * PageSize::Size4K);
            if (!fallback)
                return nullptr;

            addRange(reinterpret_cast<uint64_t>(fallback), 4 * PageSize::Size4K);
            return freelist.remove(FillMode::NONE);  // Physical::Allocate already cleared the memory
        }
        return ptr;
    }

    void deallocate(PhysicalAddress ptr) {
        if (ptr)
            freelist.add(ptr);
    };

    void addRange(uint64_t base, uint64_t len) {
        auto pages = ((base + len - 1) - base) / PageSize::Size4K;
        auto top = base + pages * PageSize::Size4K;

        if constexpr (verbose)
            ConsoleLogger::log() << ConsoleLogger::log().format("base: 0x%x | top: 0x%x | pages: %d\n", base, top, pages);

        for (auto i = 0ul; i < (len / PageSize::Size4K); i++) {
            for (auto i = 0ul; i <= pages; i++) {
                freelist.add(reinterpret_cast<PhysicalAddress>(base + (i * PageSize::Size4K)));
            }
        }
    }
};
}  // namespace firefly::kernel::mm
