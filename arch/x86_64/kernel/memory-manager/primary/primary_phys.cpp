#include "x86_64/memory-manager/primary/primary_phys.hpp"

#include <stl/cstdlib/stdio.h>

#include "x86_64/libk++/align.h"
#include "x86_64/trace/strace.hpp"

namespace firefly::kernel::mm::primary {
static libkern::Bitmap bitmap;
static uint32_t *arena;
static int64_t linked_list_allocation_base;  //Base address for the linked list structure (Should never be freed, may be reused)
static size_t linked_list_allocation_index = 0;
using libkern::align4k;


static void *early_alloc(struct stivale2_mmap_entry &entry, int size) {
    size_t ret = entry.base;
    entry.base += size;
    entry.length -= size;
    return reinterpret_cast<void *>(ret);
}

template <typename T>
T *small_alloc() {
    T *curr = reinterpret_cast<T *>(linked_list_allocation_base + linked_list_allocation_index);
    linked_list_allocation_index += sizeof(T);
    *curr = T{};
    return curr;
}

void init(struct stivale2_struct_tag_memmap *mmap) {
    bool init_ok = false;

    // The highest possible *free* entry in the mmap
    uint64_t highest_page = 0;
    uint64_t top = 0;
    for (size_t i = 0; i < mmap->entries; i++) {
        if (mmap->memmap[i].type != STIVALE2_MMAP_USABLE)
            continue;

        top = mmap->memmap[i].base + mmap->memmap[i].length - 1;
        if (top > highest_page)
            highest_page = top;
    }

    // // DEBUG: Print mmap contents
    // for (size_t i = 0; i < mmap->entries; i++) {
    //     printf("(%d) %X-%X [ %X (%s) ]\n", i, mmap->memmap[i].base, mmap->memmap[i].base + mmap->memmap[i].length - 1, mmap->memmap[i].type, mmap->memmap[i].type == 1 ? "free" : "?");
    // }

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
        size_t end = bitmap.allocator_conversion(false, mmap->memmap[i].length);
        printf("Freeing %d pages at %X\n", end, bitmap.allocator_conversion(true, base));

        for (size_t i = base; i < base + end; i++) {
            auto success = bitmap.clear(
                                     i)
                               .success;
            if (!success) {
                trace::panic("Failed to free page during primary allocator setup");
            }
        }
    }

    // Setup the base address for the linked list.
    linked_list_allocation_base = bitmap.find_first(libkern::BIT_SET);
    if (linked_list_allocation_base == -1)
        trace::panic("No free memory");

    if (!bitmap.set(linked_list_allocation_base).success)
        trace::panic("Failed to mark the primary allocators linked list as used!");
    
    linked_list_allocation_base = bitmap.allocator_conversion(true, linked_list_allocation_base);

    //NOTE: Never free bit 0 - It's a nullptr
}

/*
    Append a node at the end of the linked list
*/
__attribute__((always_inline)) inline 
void append(struct primary_allocation_result *head_ref, void *addr) {
    auto new_node = small_alloc<struct primary_allocation_result>();
    auto last = head_ref;
    new_node->addr = addr;
    new_node->next = nullptr;
    
    while (last->next != nullptr)
        last = last->next;

    last->next = new_node;
}

/*
    Primary physical allocator.
    - Allocates up to 4095 pages of physical memory
    - All addresses returned are page aligned
    - All pages are initialized to zero
    - Panics on failure
    
    Returns a linked list for possible
    non contiguous memory.
    
    Call unpack() to skip the page reserved for the linked list.
    
    IMPORTANT: You must NOT use the list returned as it is required for deallocation!
    Create a copy instead:
        auto ptr = allocate(2)->unpack();
        auto ptr2 = ptr; //Work with ptr2!
        ptr2 = ptr2->next; //etc..
        deallocate(ptr);

*/
struct primary_allocation_result *allocate(size_t pages) {
    // NOTE: There is no need to check for a single page to try and optimize
    // the allocation since this type of allocation would violate the core
    // prinicple of the kernel. (Each process has it's own allocator)
    
    // PAGE_SIZE - 1 because 1 page is dedicated to the linked list at all times
    if (pages > PAGE_SIZE-1)
        return nullptr;

    auto linked_list = small_alloc<struct primary_allocation_result>();
    linked_list->addr = reinterpret_cast<void*>(MAGIC);  // Linked list header signature, this must be skipped

    for (; pages > 0; pages--) {
        auto bit = bitmap.find_first(libkern::BIT_SET);
        if (bit == -1)
            trace::panic("No free memory!");

        bitmap.set(bit);
        
        append(linked_list, reinterpret_cast<void*>(bitmap.allocator_conversion(true, bit)));
    }

    linked_list_allocation_index = 0;  //Reset linear allocator for next phys allocation
    return linked_list;
}

void deallocate(struct primary_allocation_result *list)
{
    // List has been unpacked in the function argument (Allocation must not have called unpack() for this to work)
    // When in doubt, check the return value of unpack() or simply check the MAGIC header of addr
    // If it's 0xC0FFEE, this node may NOT be used, advance to the next one. ()
    if (list->unpack() == nullptr)
    {
        while (list != nullptr)
        {
            bitmap.clear(bitmap.allocator_conversion(false, reinterpret_cast<size_t>(list->addr)));
            list = list->next;
        }
    }

    else
    {
        list = list->unpack();
        while (list != nullptr)
        {
            bitmap.clear(bitmap.allocator_conversion(false, reinterpret_cast<size_t>(list->addr)));
            list = list->next;
        }
    }
}

}  // namespace firefly::kernel::mm::primary
