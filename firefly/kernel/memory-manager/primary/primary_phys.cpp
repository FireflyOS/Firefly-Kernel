#include "firefly/memory-manager/primary/primary_phys.hpp"

#include "firefly/memory-manager/primary/bootstrap_allocator.hpp"
#include "firefly/memory-manager/zone-specifier.hpp"

// Frigg assertions
void FRG_INTF(log)(const char *cstring) {
    firefly::kernel::info_logger << cstring;
}

void FRG_INTF(panic)(const char *cstring) {
    firefly::panic(cstring);
}

namespace firefly::kernel::mm::pmm {

using PhysicalAddress = void *;
static constexpr bool verbose = true;
static constexpr uint64_t num_mib_per_zone = 1;

frg::intrusive_list<
    Zone,
    frg::locate_member<
        Zone,
        frg::default_list_hook<Zone>,
        &Zone::next>>
    zones;

static Zone *active_zone;


// Preallocates memory for the zone structs
static BootstrapAllocator reserve_zone_memory(stivale2_struct_tag_memmap *mmap);

void init(stivale2_struct_tag_memmap *mmap) {
    int num_zones = 0;
    long total_free = 0;

    /*
        1. Calculate how many zones we need
        2. Preallocate some memory for all these zones.
        3. Initialize the zones using this preallocated memory (this way we don't take memory from the memory the zone holds in 'base')
    */

    BootstrapAllocator zone_allocator = reserve_zone_memory(mmap);

    for (auto i = 0ul; i < mmap->entries; i++) {
        auto entry = mmap->memmap[i];
        if (entry.type != STIVALE2_MMAP_USABLE)
            continue;

        auto base = entry.base;
        auto len = entry.length;

        if (len >= MiB(num_mib_per_zone)) {
            if constexpr (verbose)
                info_logger << (len / MiB(num_mib_per_zone)) << " 1MiB large zones from " << info_logger.hex(base) << " - " << info_logger.hex(base + len) << '\n';

            for (uint64_t i = 0; i < (len / MiB(num_mib_per_zone)); i++) {
                Zone *z = init_zone(base, MiB(num_mib_per_zone), num_zones++, zone_allocator);
                zones.push_back(z);

                base += MiB(num_mib_per_zone);
            }
            total_free += len;
        }
    }

    for (int i = 0; i < num_zones; i++) {
        if (zones.empty())
            panic("Zone is null\n");

        active_zone = zones.pop_back();
        info_logger << "Zone#" << active_zone->zone_id << " base: " << active_zone->base << '\n';
    }

    panic("");
    active_zone = zones.pop_front();
    info_logger << "Total free: " << total_free << " bytes (" << total_free / PAGE_SIZE << " pages)\n";
    info_logger << "pmm: Initialized " << logger::endl;
}

PhysicalAddress allocate([[maybe_unused]] FillMode fill) {
    auto zone = active_zone;
    auto ptr = zone->allocator.alloc(4096);
    // info_logger << "EEE: " << (uint64_t)zones.pop_front()->base << '\n';

    if (!ptr) {
        info_logger << zones.front()->allocator.alloc(4096) << '\n';
        active_zone = zones.pop_front();

        if (!active_zone)
            panic("OOM");

        ptr = active_zone->allocator.alloc(4096);
    }

    return ptr;
}

void deallocate([[maybe_unused]] PhysicalAddress ptr) {
    info_logger << "pmm::deallocate() is a stub!\n";
}

static uint64_t calculate_how_many_zones(stivale2_struct_tag_memmap *mmap) {
    uint64_t num_zones = 0;

    for (auto i = 0ul; i < mmap->entries; i++) {
        auto entry = mmap->memmap[i];
        if (entry.type != STIVALE2_MMAP_USABLE)
            continue;

        auto len = entry.length;

        if (len >= MiB(num_mib_per_zone)) {
            for (; num_zones < (len / MiB(num_mib_per_zone)) - 1; num_zones++)
                ;
        }
    }

    return num_zones;
}

static BootstrapAllocator reserve_zone_memory(stivale2_struct_tag_memmap *mmap) {
    // Step 1: Calculate the number of required zones.
    auto num_zones = calculate_how_many_zones(mmap);
    info_logger << "Require " << num_zones << " zones (" << num_zones * sizeof(Zone) << " bytes)\n";

    // Step 2: Find an entry in the memory map large enough to allocate all the zone structs and resize the entry to mark that memory as reserved.
    BootstrapAllocator alloc;
    auto min_required_size = num_zones * sizeof(Zone);
    libkern::align4k<uint64_t>(min_required_size);  // Ensure the memory map is still page aligned when resizing even if this means wasting some memory.

    for (auto i = 0ul; i < mmap->entries; i++) {
        auto &entry = mmap->memmap[i];
        if (entry.type != STIVALE2_MMAP_USABLE)
            continue;

        auto &len = entry.length;
        auto &base = entry.base;

        if (len >= min_required_size) {
            // alloc.init_from_zone(reinterpret_cast<BuddyAllocator::PhysicalAddress>(base), log2(min_required_size));
            alloc.init_buffer(base, min_required_size, num_zones);
            memset(reinterpret_cast<void *>(base), 0, min_required_size);
            base += PAGE_SIZE;
            len -= PAGE_SIZE;
            break;
        }
    }
    return alloc;
}

}  // namespace firefly::kernel::mm::pmm