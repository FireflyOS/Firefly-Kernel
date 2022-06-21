#include "firefly/memory-manager/primary/primary_phys.hpp"

#include <frg/algorithm.hpp>
#include <frg/array.hpp>
#include <frg/vector.hpp>

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

static PhysicalAddress do_zone_allocate(uint64_t size, FillMode fill);

// Todo: This should probably just take the largest zone or something.
// Maybe even calculate the best fitting zone and use that instead of the 'zones' list
class VectorAllocator {
public:
    void *allocate(size_t size) const {
        return pmm::do_zone_allocate(size, FillMode::NONE);
    }
    void free([[maybe_unused]] void *ptr) const {
        info_logger << "VectorAllocator: free() is a stub!\n";
    }
};

static Zone *active_zone;
frg::vector<Zone *, VectorAllocator> trees_ordering;

// std::vector<BuddyTree> trees = initialize(); // instantiate them all (this would 'zones')
// std::vector<BuddyTree*> trees_ordering;      // this would be 'zones' but sorted by size (largest->smallest)
// this contains all trees in `trees`, but ordered by their largest free zone.
// a `BuddyTree` contains a `size_t`, current index in trees_ordering, and then you can swap them around
// because you can just easily fix the sorting in `trees_ordering`, then you can keep track of where the sizes start
// in the following

static constexpr auto tree_ordering_array_sz = BuddyAllocator::largest_allowed_order - BuddyAllocator::smallest_allowed_order;
[[maybe_unused]] static constexpr int tree_ordering_full = tree_ordering_array_sz + 1;  // trees_ordering_start[array_size + 1] points to unusable (fully allocated) memory
[[maybe_unused]] static constexpr int tree_ordering_largest = tree_ordering_array_sz;   // points to the largest allocation size
[[maybe_unused]] static constexpr int tree_ordering_smallest = 0;                       // points to the smallest allocation size
[[maybe_unused]] static constexpr int tree_ordering_invalid = -1;                       // there is no allocatable size here And there never will be.
                                                                                        // An example would be a machine with 512mb, but the max size of a buddy allocator is 1gb.
                                                                                        // Therefore some entries in 'trees_ordering_start' would be -1.

frg::array<int64_t, tree_ordering_array_sz + 1> trees_ordering_start;
// so saying tree_ordering_array_sz (i.e. "ORDER_COUNT" or "num_orders") == 2, and you have
// { 0, 600} contained, that means that trees_ordering_start[:600] contains order 0 free, and 600 is where order 1 starts
//
// In other words (still assuming order_count == 2):
//  trees_ordering_start[min_sz ] -> value at this index is: 10 -> trees_ordering[10]: elements 10 - 20 are of size N
//  trees_ordering_start[max_sz ] -> value at this index is: 20 -> trees_ordering[20]: elements 20 - 21 are of size N
//  trees_ordering_start[full_sz] -> value at this index is: 21 -> trees_ordering[21]: elements 21 - ?? are of size N
//

// Preallocates memory for the zone structs
static BootstrapAllocator reserve_zone_memory(stivale2_struct_tag_memmap *mmap);

void init(stivale2_struct_tag_memmap *mmap) {
    int num_zones = 0;

    /*
        1. Calculate how many zones we need
        2. Preallocate some memory for all these zones.
        3. Initialize the zones using this preallocated memory (this way we don't take memory from the memory the zone holds in 'base')
        4. Place the zones (and their respective buddy allocators) into a vector.
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
        for (uint64_t i = 0; i < 64; i++) {
            if (len & (1LL << i)) {
                Zone *z = init_zone(base, (1LL << i), ++num_zones, zone_allocator, verbose);
                zones.push_front(z);
                base += (1LL << i);
            }
        }
    }

    active_zone = zones.pop_front();
    info_logger << "Active zone: id=" << active_zone->zone_id << "/" << num_zones << " base-top=" << info_logger.hex(active_zone->base) << " - " << info_logger.hex(active_zone->top) << " [" << active_zone->page_count << " pages]\n";

    // Sort vector by the Zone's order
    for (auto x : zones) {
        info_logger << "zone#" << x->zone_id << " order: " << x->order << '\n';
        trees_ordering.push(x);
    }

    frg::insertion_sort(trees_ordering.begin(), trees_ordering.end(), [](auto a, auto b) {
        return a->order < b->order;
    });

    for (auto x : trees_ordering)
        info_logger << "zone#" << x->zone_id << ", idx: " << x->index_in_tree_ordering << '\n';

    info_logger << "pmm: Initialized " << logger::endl;
    
    for (;;)
        ;
}

// Need this for the VectorAllocator
static PhysicalAddress do_zone_allocate(uint64_t size, FillMode fill) {
    auto zone = active_zone;
    auto ptr = zone->allocator.alloc(size, fill);

    if (!ptr) {
        info_logger << "Completely allocated zone#" << active_zone->zone_id << " : " << info_logger.hex(active_zone->base) << '\n';

        if (zones.empty())
            panic("OOM");

        active_zone = zones.pop_front();
        ptr = active_zone->allocator.alloc(size, fill);
    }

    return ptr;
}

// Note: This will have different logic than 'do_zone_allocate()'
PhysicalAddress allocate(uint64_t size, FillMode fill) {
    // NOTE: This logic is temporary.

    auto zone = active_zone;
    auto ptr = zone->allocator.alloc(size, fill);

    if (!ptr) {
        info_logger << "Completely allocated zone#" << active_zone->zone_id << " : " << info_logger.hex(active_zone->base) << '\n';

        if (zones.empty())
            panic("OOM");

        active_zone = zones.pop_front();
        ptr = active_zone->allocator.alloc(size, fill);
    }

    return ptr;
}

// Can't implement deallocate(), because if a zone is OOM it's
// just discarded and not stored anywhere as of now.
//
// For more information see the comment in allocate().
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
            if (len & (1LL << i)) {
                num_zones++;
            }
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

        if (len >= min_required_size && base > 0) {
            alloc.init_buffer(base, sizeof(Zone), min_required_size);

            base += min_required_size;
            len -= min_required_size;
            break;
        }
    }
    return alloc;
}

}  // namespace firefly::kernel::mm::pmm