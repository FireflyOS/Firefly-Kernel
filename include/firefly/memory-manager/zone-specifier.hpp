#pragma once

#include <cstdlib/cmath.h>

#include <cstddef>
#include <frg/list.hpp>

#include "firefly/memory-manager/mm.hpp"
#include "firefly/memory-manager/primary/buddy.hpp"
#include "firefly/memory-manager/primary/bootstrap_allocator.hpp"
#include "libk++/align.h"
#include "libk++/bits.h"

namespace firefly::kernel::mm {

using ZoneId = int;

enum ZoneType {
    ZONE_TYPE_LOW = 1 << 0,  // Conventional memory
    ZONE_TYPE_HIGH = 1 << 1  // Higher half memory
};

struct Zone {
    BuddyAllocator allocator;

    size_t base;
    size_t top;
    size_t page_count;
    size_t length;
    ZoneId zone_id;
    enum ZoneType zone_type;
    frg::default_list_hook<Zone> next;
};

[[gnu::used]] static Zone *init_zone(uint64_t &base, uint64_t len, int nr, BootstrapAllocator allocator, ZoneType type = ZONE_TYPE_LOW) {
    Zone *zone = (Zone *)allocator.allocate_buffer();
    if (!zone)
        panic("Failed to allocate memory for a zone structure (Try increasing `BootstrapAllocator.largest_size`)\n");

    // Some frigg assertions may fail due to the random values in uninitialized memory.
    // So we clear it to avoid any UB (this bug took a while to figure out lol)
    memset((void *)zone, 0, sizeof(Zone));

    // info_logger << info_logger.format("zone: 0x%X-0x%X\n", base, base + len);
    zone->base = base;
    zone->length = len;
    zone->page_count = len / PAGE_SIZE;
    zone->zone_id = nr;
    zone->zone_type = type;

    auto top = base + len;
    long size_bytes = (top - base);
    auto suitable_order = log2(size_bytes);

    // info_logger << "Order for size " << size_bytes << " is: " << suitable_order << '\n';
    zone->allocator.init_from_zone(reinterpret_cast<BuddyAllocator::PhysicalAddress>(base), suitable_order);

    return zone;
}

}  // namespace firefly::kernel::mm