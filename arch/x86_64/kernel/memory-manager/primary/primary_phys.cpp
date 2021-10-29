#include "x86_64/memory-manager/primary/primary_phys.hpp"

#include <stl/cstdlib/stdio.h>

#include "x86_64/libk++/align.h"
#include "x86_64/trace/strace.hpp"

namespace firefly::kernel::mm::primary {
static libkern::Bitmap bitmap;
static uint32_t *arena;
using libkern::align4k;


static void *early_alloc(struct stivale2_mmap_entry &entry, int size) {
    size_t ret = entry.base;
    entry.base += size;
    entry.length -= size;
    return reinterpret_cast<void *>(ret);
}

void allocate();

void init(struct stivale2_struct_tag_memmap *mmap) {
    bool init_ok = false;

    // The highest possible *free* entry in the mmap
    uint64_t highest_page = 0;
    uint64_t top = 0;
    for (size_t i = 0; i < mmap->entries; i++) {
        if (mmap->memmap[i].type != STIVALE2_MMAP_USABLE)
            continue;

        top = mmap->memmap[i].base + mmap->memmap[i].length - 1;
        printf("%X\n", top);
        if (top > highest_page)
            highest_page = top;
    }

    // DEBUG: Print mmap contents
    for (size_t i = 0; i < mmap->entries; i++) {
        printf("(%d) %X-%X [ %X (%s) ]\n", i, mmap->memmap[i].base, mmap->memmap[i].base + mmap->memmap[i].length - 1, mmap->memmap[i].type, mmap->memmap[i].type == 1 ? "free" : "?");
    }

    size_t bitmap_size = (highest_page / PAGE_SIZE / 8);
    align4k<size_t>(bitmap_size);
    printf("bitmap sz: %X - %d\n", bitmap_size, bitmap_size);

    // Iterate through mmap and find largest block to store the bitmap
    for (size_t i = 0; i < mmap->entries; i++) {
        if (mmap->memmap[i].type != STIVALE2_MMAP_USABLE)
            continue;

        if (mmap->memmap[i].length >= bitmap_size) {
            printf("Found entry to store the bitmap (%d bytes) at %X-%X\n", bitmap_size, mmap->memmap[i].base, mmap->memmap[i].base + mmap->memmap[i].length - 1);

            // Note: The entire memory contents are marked as used now, we free available memory after this
            arena = reinterpret_cast<uint32_t *>(early_alloc(mmap->memmap[i], bitmap_size));
            bitmap.init(arena, bitmap_size);
            bitmap.setall();
            init_ok = true;

            // Note: There is no need to resize this entry since `early_alloc` already did.
            break;
        }
    }
    if (!init_ok) {
        trace::panic("Failed to initialize the primary allocation structure");
    }

    for (size_t i = 0; i < mmap->entries; i++) {
        if (mmap->memmap[i].type != STIVALE2_MMAP_USABLE)
            continue;

        size_t base = bitmap.allocator_conversion(false, mmap->memmap[i].base);
        size_t end  = bitmap.allocator_conversion(false, mmap->memmap[i].length);
        printf("Freeing %d pages at %X\n", end, bitmap.allocator_conversion(true, base));

        for (size_t i = base; i < base + end; i++) {
            auto success = bitmap.clear(
                i
            ).success;
            if (!success) { trace::panic("Failed to free page during primary allocator setup"); }
        }
    }
    //NOTE: Never free bit 0 - It's a nullptr
}

void allocate()
{
    auto bit = bitmap.find_first(libkern::BIT_SET);
    if (bit == -1)
        trace::panic("no free memory!");
    else
        printf("Found free page at bit %d (%X)\n", bit, bit * PAGE_SIZE);
}

}  // namespace firefly::kernel::mm::primary
