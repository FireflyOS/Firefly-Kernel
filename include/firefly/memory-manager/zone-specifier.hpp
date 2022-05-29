#pragma once

#include <cstddef>
#include <frg/list.hpp>

#include "firefly/memory-manager/mm.hpp"
#include "libk++/align.h"

namespace firefly::kernel::mm {

using ZoneId = int;

enum ZoneType {
    ZONE_TYPE_LOW = 1 << 0,  // Conventional memory
    ZONE_TYPE_HIGH = 1 << 1  // Higher half memory
};

struct Zone {
    PhysicalAddress base;
    PhysicalAddress top;
    size_t page_count;
    size_t length;
    ZoneId zone_id;
    enum ZoneType zone_type;
    frg::default_list_hook<Zone> next;
};

static Zone *init_zone(size_t base, size_t length, ZoneId id, ZoneType type = ZONE_TYPE_LOW) {
    // We have to remove one whole page from each zone to allocate memory for the
    // Zone struct itself, so we made sure this won't be an empty zone.
    if (base + length <= 4096) {
        return nullptr;
    }

    Zone *zone = reinterpret_cast<Zone *>(base);

    // Todo: Maybe we can do 'base += sizeof(Zone)' and give
    // the left over bytes until the next page* to the slab allocator    *The number of bytes left would be: '(base + PAGE_SIZE) - sizeof(Zone)'
    base += PAGE_SIZE;
    length -= PAGE_SIZE;

    info_logger << info_logger.format("zone: 0x%X-0x%X\n", base, base + length);
    zone->base = reinterpret_cast<PhysicalAddress>(base);
    zone->top = reinterpret_cast<PhysicalAddress>(base + length - 1);
    zone->page_count = length / PAGE_SIZE;
    zone->length = length;
    zone->zone_id = id;
    zone->zone_type = type;

    return zone;
}

}  // namespace firefly::kernel::mm