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
static constexpr bool verbose{};

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

        // Split the memory map into sizes of powers of two
        // This ensures we waste as little memory as possible
        for (int i = 0; i < 64; i++) {
            long long x = 1;

            if (len & (x << i)) {
                Zone *z = init_zone(base, 1 << i, num_zones++, zone_allocator, verbose);
                zones.push_back(z);
                base += 1 << i;
            }
        }
    }
    --num_zones; // zones are zero indexed, so we remove one excess refcount

    active_zone = zones.pop_front();
    info_logger << "Active zone: id=" << active_zone->zone_id << "/" << num_zones << " base-top=" << info_logger.hex(active_zone->base) << " - " << info_logger.hex(active_zone->top) << " [" << active_zone->page_count << " pages]\n";
    info_logger << "pmm: Initialized " << logger::endl;
}

PhysicalAddress allocate([[maybe_unused]] FillMode fill) {
    // NOTE: This logic is temporary.
    // This function should have a size parameter and search
    // the 'zones' list for a zone that is large enough to perform
    // this allocation.
    //
    // This function should proceed to ask the buddy for 'size' bytes
    // of memory and find a new zone if the buddy returned a nullptr.
    // Also, if a buddy is exhausted(OOM) it should be stored in a separate list
    // for full zones to help speed up allocations and deallocations.

    auto zone = active_zone;
    auto ptr = zone->allocator.alloc(4096);

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

static uint64_t calculate_num_zones(stivale2_struct_tag_memmap *mmap) {
    uint64_t num_zones = 0;

    for (auto i = 0ul; i < mmap->entries; i++) {
        auto entry = mmap->memmap[i];
        if (entry.type != STIVALE2_MMAP_USABLE)
            continue;

        auto len = entry.length;

        for (int i = 0; i < 64; i++) {
            long long x = 1;

            if (len & (x << i))
                ++num_zones;
        }
    }

    return num_zones;
}

static BootstrapAllocator reserve_zone_memory(stivale2_struct_tag_memmap *mmap) {
    // Step 1: Calculate the number of required zones.
    auto num_zones = calculate_num_zones(mmap);
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