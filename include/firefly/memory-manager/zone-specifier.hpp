// #pragma once

// #include <cstdlib/cmath.h>

// #include <cstddef>
// #include <frg/list.hpp>

// #include "firefly/memory-manager/mm.hpp"
// #include "firefly/memory-manager/primary/bootstrap_allocator.hpp"
// #include "firefly/memory-manager/primary/buddy.hpp"
// #include "libk++/align.h"
// #include "libk++/bits.h"

// namespace firefly::kernel::mm {

// using ZoneId = int;

// enum ZoneType {
//     ZONE_TYPE_LOW = 1 << 0,  // Conventional memory
//     ZONE_TYPE_HIGH = 1 << 1  // Higher half memory
// };

// struct Zone {
//     BuddyAllocator allocator;
//     BuddyAllocator::Order order;

//     size_t index_in_tree_ordering;
//     size_t base;
//     size_t top;
//     size_t page_count;
//     size_t length;
//     ZoneId zone_id;
//     enum ZoneType zone_type;
//     frg::default_list_hook<Zone> next;
// };

// [[gnu::used]] static Zone *init_zone(uint64_t &base, uint64_t len, int nr, BootstrapAllocator &allocator, const bool verbose = false, ZoneType type = ZONE_TYPE_LOW) {
//     Zone *zone = (Zone *)allocator.allocate_buffer();

//     // This shouldn't happen. If it does, something went *really* wrong
//     if (!zone) {
//         if (verbose)
//             info_logger << "WARNING: Bogus address for zone#" << nr << " | Discarding...\n";
//         return nullptr;
//     }

//     // Some frigg assertions (intrusive list) may fail due to the random values in uninitialized memory.
//     // So we clear it to avoid any UB (this bug took a while to figure out lol)
//     memset((void *)zone, 0, sizeof(Zone));

//     if (verbose)
//         info_logger << "Zone#" << nr << " " << info_logger.hex(base) << " - " << info_logger.hex(base + len) << '\n';

//     zone->base = base;
//     zone->top = base + len;
//     zone->length = len;
//     zone->page_count = len / PAGE_SIZE;
//     zone->zone_id = nr;
//     zone->zone_type = type;

//     auto top = base + len;
//     long size_bytes = (top - base);
//     auto suitable_order = zone->order = log2(size_bytes);

//     // Calculate the index of this zone in 'trees_ordering_start'
//     size_t normalized_order = BuddyAllocator::largest_allowed_order - BuddyAllocator::smallest_allowed_order;
//     zone->index_in_tree_ordering = normalized_order - (BuddyAllocator::largest_allowed_order - suitable_order);
//     assert_truth(zone->index_in_tree_ordering >= 0 && zone->index_in_tree_ordering <= normalized_order);

//     if (verbose)
//         info_logger << "Order for size " << size_bytes << " is: " << suitable_order << '\n';

//     zone->allocator.init_from_zone(reinterpret_cast<BuddyAllocator::PhysicalAddress>(base), suitable_order);

//     return zone;
// }

// }  // namespace firefly::kernel::mm