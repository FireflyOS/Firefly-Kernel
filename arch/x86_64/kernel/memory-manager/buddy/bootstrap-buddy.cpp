#include "x86_64/memory-manager/buddy/bootstrap-buddy.hpp"
#include "x86_64/trace/strace.hpp"
#include <stl/cstdlib/stdio.h>

namespace firefly::kernel::mm::buddy {
uint64_t ram_lowest, ram_highest = 0x0;

struct limine_mem {
    uint64_t base;
    uint64_t end;
};

static void dump_mmap(struct stivale2_mmap_entry *mmap_entries, int total_entries);
static struct limine_mem limine_mmap(struct stivale2_mmap_entry *mmap_entries, int total_entries, size_t buddy_size);

void bootstrap_buddy(struct stivale2_struct_tag_memmap *phys_mmap) {
    /*
    *  High level walkthrough on the setup of the buddy allocator
    * 
    *   1. Get the abs min and max address of the mmap and return the highest address in the mmap
    *   2. Calculate the amount of memory the buddy structure requires
    *   3. Find an entry in the mmap that is large enough to host the buddy allocator
    *   4. Initialize the buddy allocator
    *   5. Lastly the used entries are allocated
    */
    
    dump_mmap(phys_mmap->memmap, phys_mmap->entries);
    
    auto total_mem = ram_diff(phys_mmap->memmap, phys_mmap->entries);
    auto memory_used = BuddyAllocator(nullptr).estimate_memory_used(total_mem);
    struct limine_mem mmap = limine_mmap(phys_mmap->memmap, phys_mmap->entries, memory_used);
    BuddyAllocator Buddy = { (void*)mmap.base };
    printf("mmap.base: 0x%X\nmmap.end: 0x%X\n", mmap.base, mmap.end);
    printf("before init\n");
    Buddy.initialize(total_mem, (char*) ram_lowest);
    // Buddy.set_unusable_memory((char*)mmap.base, (char*)mmap.end);
    printf("after init\n");
}

static inline uint64_t ram_diff(struct stivale2_mmap_entry *mmap_entries, int total_entries) {
    ram_lowest = mmap_entries[0].base;
    ram_highest = mmap_entries[total_entries-1].base + mmap_entries[total_entries-1].length - 1;
    printf("ram_low: %X | ram_high: %X | ram total: %X\n", ram_lowest, ram_highest, ram_highest - ram_lowest);
    return ram_highest;
}

static struct limine_mem limine_mmap(struct stivale2_mmap_entry *mmap_entries, int total_entries, size_t buddy_size) {
    size_t result = 0;

    for (int current_entry = 0; current_entry < total_entries; current_entry++)
    {
        // struct stivale2_mmap_entry *entry = &mmap_entries[current_entry];
        if (mmap_entries[current_entry].type != STIVALE2_MMAP_USABLE)
            continue;
        
        if ((result = (mmap_entries[current_entry].base + mmap_entries[current_entry].length)) >= buddy_size) {
            printf("buddy_size: %X\n", buddy_size);
            printf("Internal allocator base-length: %X-%X\n", result, result + mmap_entries[current_entry].length-1);
            return {result, result + mmap_entries[current_entry].length-1};
        }
    }

    trace::panic("Cannot find a large enough entry to host the buddy allocator");
    __builtin_unreachable(); //Avoids the "not all code paths return a value" compiler error
}

static void dump_mmap(struct stivale2_mmap_entry *mmap_entries, int total_entries) {
    for (int current_entry = 0; current_entry < total_entries; current_entry++)
    {
        printf("(%d) %X-%X [%X]\n", current_entry, mmap_entries[current_entry].base, mmap_entries[current_entry].base + mmap_entries[current_entry].length-1, mmap_entries[current_entry].type);
    }
}

}  // namespace firefly::kernel::mm::buddy
