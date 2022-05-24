#pragma once

#include <stdint.h>
#include <frg/list.hpp>

#include "x86_64/libk++/align.h"
#include "x86_64/compiler/clang++.hpp"
#include "x86_64/stivale2.hpp"
#include <stl/cstdlib/stdio.h>

void FRG_INTF(panic)(const char *cstring)
{
    printf("%s\n", cstring);
}

void FRG_INTF(log)(const char *cstring)
{
    printf("%s\n", cstring);
}

#define InvalidAddress nullptr

namespace firefly::kernel::mm
{
    constexpr uint32_t PAGE_SIZE = 4096;

    class pmm
    {
    public:
        using PhysicalAddress = void *;

        struct FreelistNode
        {
            PhysicalAddress pointer;
            frg::default_list_hook<FreelistNode> next;
        };

        static frg::intrusive_list<
            FreelistNode,
            frg::locate_member<FreelistNode, frg::default_list_hook<FreelistNode>, &FreelistNode::next>>
            freelist;

        static void init(stivale2_struct_tag_memmap *mmap)
        {
            for (auto i = 0ul; i < mmap->entries; i++)
            {
                auto &entry = mmap->memmap[i];
                if (entry.type != STIVALE2_MMAP_USABLE)
                    continue;

                auto &base = entry.base;
                auto &len = entry.length;
                auto pages = ((base + len - 1) - base) / PAGE_SIZE;
                auto top = base + pages * PAGE_SIZE;
                printf("base: 0x%x | top: 0x%x | pages: %d\n", base, top, pages);
                auto freelist_base = base;

                // Preallocate some memory for the freelists elements
                for (auto i = 0ul; i < pages; i++)
                    base += sizeof(FreelistNode);

                libkern::align4k<uint64_t>(base);
                if (base >= top)
                    continue;

                auto wasted_pages = (base - freelist_base) / PAGE_SIZE;
                pages = pages - wasted_pages;

                printf("Freelist base: 0x%x | Base: 0x%x | Top: 0x%x | Pages left: %d\n", freelist_base, base, top, pages);
                // printf("Number of pages wasted on freelist: %d\n", wasted_pages);

                for (auto i = 0ul; i < pages; i++)
                {
                    auto node = reinterpret_cast<FreelistNode *>(freelist_base);
                    node->pointer = reinterpret_cast<PhysicalAddress>(base);
                    freelist_base += sizeof(FreelistNode);

                    printf("node: 0x%x | node->ptr: 0x%x\n", node, node->pointer);

                    printf("Empty: %d\n", freelist.empty());
                    freelist.push_back(node);

                    base += PAGE_SIZE;
                }
            }
        }

        static PhysicalAddress allocate()
        {
            if (freelist.empty())
                return InvalidAddress;

            auto res = freelist.pop_front();
            return res->pointer;
        }

        static void deallocate([[maybe_unused]] PhysicalAddress ptr)
        {
            // TODO: We need to use struct zones for each mmap entry.
            // It's structure would look something like this:
            //
            //  struct zone {
            //    uint64_t freelist_base; <--- Base address of the FreelistNodes (See: init())
            //    uint64_t base, top;     <--- Base and top allocatable address
            //  };
            // Given this information we can then put a FreelistNode back on the freelist using a 'PhysicalAddress' rather than a 'FreelistNode'
            //  Calculation (untested): (ptr - base) - freelist_base
        }
    };
} // namespace firefly::kernel::mm::primary

