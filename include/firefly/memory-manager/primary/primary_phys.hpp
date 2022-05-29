#pragma once

#include <cstdlib/cstring.h>
#include <stdint.h>

#include <frg/list.hpp>

#include "firefly/logger.hpp"
#include "firefly/memory-manager/mm.hpp"
#include "firefly/memory-manager/zone-specifier.hpp"
#include "firefly/stivale2.hpp"
#include "firefly/trace/strace.hpp"

// Since the pmm class has only static members and functions they need to be written out once.
// This is just a wrapper to make this easier and cleaner
#define COMPILE_TIME_CONFIGURE_PMM_CLASS                       \
    frg::intrusive_list<                                       \
        firefly::kernel::mm::Zone,                             \
        frg::locate_member<                                    \
            firefly::kernel::mm::Zone,                         \
            frg::default_list_hook<firefly::kernel::mm::Zone>, \
            &firefly::kernel::mm::Zone::next>>                 \
        firefly::kernel::mm::pmm::zones;                       \
                                                               \
    firefly::kernel::mm::pmm::Freelist<firefly::kernel::mm::pmm::PhysicalAddress> firefly::kernel::mm::pmm::freelist  // NOTE: Deprecation inbound for this

namespace firefly::kernel::mm {

class pmm {
public:
    using PhysicalAddress = void *;
    static constexpr bool verbose = false;

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
        int num_zones = 0;

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

            auto zone = init_zone(base, len, num_zones++);
            zones.push_back(zone);
        }
        info_logger << "pmm: Initialized " << logger::endl;
    }

    static PhysicalAddress allocate(FillMode fill = FillMode::ZERO) {
        return freelist.remove(fill);
    }

    static void deallocate(PhysicalAddress ptr) {
        freelist.add(ptr);
    }

    static frg::intrusive_list<
        Zone,
        frg::locate_member<
            Zone,
            frg::default_list_hook<Zone>,
            &Zone::next>>
        zones;
};
}  // namespace firefly::kernel::mm