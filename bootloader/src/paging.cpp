#include <copy_unreal.hpp>
#include <err.hpp>
#include <paging.hpp>
#include <stdio.hpp>
#include <string.hpp>

// reuse the same buffers
extern unsigned char *cluster_buffer;

/**
 *                          Identity maps all of the kernel and modules to
 *                          the lower and higher virtual address space.
 */
void setup_pages(unsigned long binary_size) {
    page_map *map_buffer = reinterpret_cast<page_map *>(cluster_buffer);
    page_table *table_buffer = reinterpret_cast<page_table *>(cluster_buffer);

    unsigned long kernel_end = 0x00100000 + binary_size;

    memset(map_buffer, 0, 4096);

    // identity map to lower and higher virtual address space
    // pml4
    map_buffer->entries[0] = map_buffer->entries[511] = { true,
                                                          true,
                                                          false,
                                                          false,
                                                          false,
                                                          false,
                                                          0,
                                                          0,
                                                          0,
                                                          0x00021ul,
                                                          0 };
    copy_unreal(static_cast<unsigned short>(reinterpret_cast<unsigned int>(map_buffer)), 0x00020000, 4096);
    memset(map_buffer, 0, 4096);

    // pdpt
    map_buffer->entries[0] = map_buffer->entries[510] = { true,
                                                          true,
                                                          false,
                                                          false,
                                                          false,
                                                          false,
                                                          0,
                                                          0,
                                                          0,
                                                          0x00022ul,
                                                          0 };
    copy_unreal(static_cast<unsigned short>(reinterpret_cast<unsigned int>(map_buffer)), 0x00021000, 4096);
    memset(map_buffer, 0, 4096);

    unsigned long target = 0;
    unsigned long dest = 0x00023000;

    // Iterate until we've mapped all of the kernel, or until we've filled the entire 64K segment with tables
    // which would allow for a 25MiB kernel + modules.
    for (unsigned int pd_entry_idx = 0; target < kernel_end && dest < 0x0002f000; pd_entry_idx++, dest += 0x1000) {
        // pd
        page_map_entry pd_entry = { true,
                                    true,
                                    false,
                                    false,
                                    false,
                                    false,
                                    0,
                                    0,
                                    0,
                                    dest >> 12,
                                    0 };
        copy_unreal(static_cast<unsigned short>(reinterpret_cast<unsigned int>(&pd_entry)), 0x00022000ul + (8 * pd_entry_idx), 8);

        for (int pt_entry = 0; pt_entry < 512 && target < kernel_end; pt_entry++, target += 4096) {
            // pt
            table_buffer->entries[pt_entry] = { true, true, false, false, false, false, false, false, 0, target >> 12, 0 };
        }
        copy_unreal(static_cast<unsigned short>(reinterpret_cast<unsigned int>(map_buffer)), dest, 4096);
    }

    // already used last 4K table and still not mapped all of kernel
    if (dest == 0x0002f000 && target < kernel_end)
        err("kernel and modules are larger than 25MiB");
}