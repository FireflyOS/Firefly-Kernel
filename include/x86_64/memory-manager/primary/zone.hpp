#pragma once

#include "x86_64/libk++/IntrusiveList.hpp"
#include "x86_64/libk++/align.h"
#include "x86_64/memory-manager/mm.hpp"
#include <stddef.h>
#include <stdint.h>

struct zone
{
    slist next;
    uint8_t *bitmap;

    int zone_id;
    size_t base;
    size_t len;
    size_t page_count;
};

class Zone
{
    static zone *init(size_t base, size_t len, int zone_id)
    {
        zone *zone = base;

        base = firefly::libkern::align4k<size_t>(base + sizeof(struct zone));
        len -= PAGE_SIZE;

        zone->base = base;
        zone->len = len;
        zone->page_count = len / PAGE_SIZE;
        zone->zone_id = zone_id;

        return zone;
    }
};