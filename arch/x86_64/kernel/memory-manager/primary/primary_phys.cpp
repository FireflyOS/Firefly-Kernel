#include "x86_64/memory-manager/primary/primary_phys.hpp"
#include <stl/cstdlib/stdio.h>

namespace firefly::kernel::mm::primary {
    // p = primary, I'm just hacking this together while avoiding naming collisions
    static struct free_list *pfree;
    static struct used_list *pused;
    
    static void *early_alloc(struct stivale2_mmap_entry entry, int size)
    {
        entry.base += size;
        entry.length -= size;
        return reinterpret_cast<void*>(entry.base);
    }

    void init(struct stivale2_struct_tag_memmap *mmap)
    {
        // The highest possible *free* entry in the mmap
        size_t highest_page = 0;
        for (size_t i = 0; i < mmap->entries; i++)
        {
            if (mmap->memmap[i].type != STIVALE2_MMAP_USABLE)
                continue;
            
            highest_page = mmap->memmap[i].base + mmap->memmap[i].length - 1;
        }

        size_t bitmap_size = (highest_page / PAGE_SIZE / 8) * 2; // We multiply by two because we have 2 bitmaps (free, used)

        // Iterate through mmap and find largest block to store the bitmaps (both used and free are bitmaps)
        for (size_t i = 0; i < mmap->entries; i++)
        {
            if (mmap->memmap[i].type != STIVALE2_MMAP_USABLE)
                continue;

            if (mmap->memmap[i].base + mmap->memmap[i].length - 1 >= bitmap_size)
            {
                printf("Found entry to store the bitmaps (%d bytes) at %X-%X\n", bitmap_size, mmap->memmap[i].base, mmap->memmap[i].base + mmap->memmap[i].length - 1);
                
                // Note: The entire memory contents are marked as used now, we free available memory after this
                pfree = reinterpret_cast<struct free_list*>(early_alloc(mmap->memmap[i], bitmap_size / 2)); // Divide by two to get the size of *one* bitmap
                pfree->bitmap_freelist.init(reinterpret_cast<libkern::bitmap_t*>(pfree), bitmap_size / 2);
                pfree->bitmap_freelist.purge();

                pused = reinterpret_cast<struct used_list*>(early_alloc(mmap->memmap[i], bitmap_size / 2));
                pused->bitmap_usedlist.init(reinterpret_cast<libkern::bitmap_t*>(pused), bitmap_size / 2);
                pused->bitmap_usedlist.setall();

                break;
            }
        }
    }

}  // namespace firefly::kernel::mm::primary