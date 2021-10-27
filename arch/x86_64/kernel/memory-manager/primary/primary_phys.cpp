#include "x86_64/memory-manager/primary/primary_phys.hpp"
#include "x86_64/libk++/bitmap.h"

namespace firefly::kernel::mm::primary {
    // p = primary, I'm just hacking this together while avoiding naming collisions
    static struct free_list *pfree;
    // static struct used_list *pused;

    static void *early_alloc(struct stivale2_mmap_entry entry, int size)
    {
        entry.base += size;
        entry.length -= size;
        return reinterpret_cast<void*>(entry.base);
    }

    void init(struct stivale2_struct_tag_memmap *mmap)
    {
        (void)mmap;
        // TODO: Iterate through mmap and find largest block to host the bitmaps (both used and free are bitmaps)

        pfree->bitmap = (uint32_t*)early_alloc(mmap->memmap[0], 10); //TEMP: Don't actually trust this entry to be usable and large enough
        libkern::Bitmap bitmap_freelist(reinterpret_cast<libkern::bitmap_t*>(pfree), 10);
        bitmap_freelist.set(0);
        bitmap_freelist.set(1);
        bitmap_freelist.set(2);
        bitmap_freelist.set(3);
        bitmap_freelist.clear(3);

        for (int i = 0; i < 4; i++)
        {
            bitmap_freelist.print(i);
        }
    }

}  // namespace firefly::kernel::mm::primary