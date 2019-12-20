#pragma once

void setup_pages(unsigned long kernel_size);

/**
 *                      Anatomy of a virtual address.
 */
struct __attribute__((packed)) virtual_address {
    /**
     *                  Zero
     */
    unsigned rsv0 : 12;

    /**
     *                  Index shorto a page table
     */
    unsigned idx_pt : 9;

    /**
     *                  Index shorto a page directory
     */
    unsigned idx_pd : 9;

    /**
     *                  Index shorto a page directory poshorter table
     */
    unsigned idx_pdpt : 9;

    /**
     *                  Index shorto a page map level 4 table
     */
    unsigned idx_pml4 : 9;

    /**
     *                  Unused
     */
    unsigned rsv1 : 16;
};

static_assert(8 == sizeof(virtual_address), "virtual_address size is incorrect");

/**
 *                      Structure of a level 2-4 page map entry.
 *                      For our purposes these 3 levels have the same structure
 */
struct __attribute__((packed)) page_map_entry {
    /**
     * true             Entry is valid
     */
    bool present : 1;
    /**
     * true             Writes are allowed to the region referenced
     */
    bool rw : 1;

    /**
     * true             User mode accesses are allowed to the region referenced
     */
    bool user : 1;

    /**
     *                  Page-level write through
     */
    bool pwt : 1;

    /**
     *                  Page-level cache disable
     */
    bool pcd : 1;

    /**
     *  true            Entry has been used in a translation
     */
    bool accessed : 1;

    /**
     *                  Ignored
     */
    unsigned ign0 : 1;

    /**
     *                  Zero
     */
    unsigned rsv0 : 1;

    /**
     *                  Ignored
     */
    unsigned ign1 : 4;

    /**
     *                  4KB-aligned page map or page table address 
     */
    unsigned long long addr : 40;

    /**
     *                  Ignored
     */
    unsigned ign2 : 12;
};

static_assert(8 == sizeof(page_map_entry), "page_map_entry size is incorrect");

/**
 *                      Page map level 2-4 structure
 */
struct __attribute__((packed)) page_map {
    page_map_entry entries[512];
};

static_assert(4096 == sizeof(page_map), "page_map size is incorrect");

/**
 *                      Structure of a page map level 1 entry
 */
struct __attribute__((packed)) page_table_entry {
    /**
     * true             Entry is valid
     */
    bool present : 1;
    /**
     * true             Writes are allowed to the 1GB region referenced
     */
    bool rw : 1;

    /**
     * true             User mode accesses are allowed to the 1GB region referenced
     */
    bool user : 1;

    /**
     *                  Page-level write through
     */
    bool pwt : 1;

    /**
     *                  Page-level cache disable
     */
    bool pcd : 1;

    /**
     *  true            Page has been accessed
     */
    bool accessed : 1;

    /**
     * true             This page has been written to
     */
    bool dirty : 1;

    /**
     *                  memory type used to access the 4KiB page
     */
    bool pat : 1;

    /**
     *                  Ignored
     */
    unsigned ign1 : 4;

    /**
     *                  physical address of 4KB page
     */
    unsigned long long addr : 40;

    /**
     *                  Ignored
     */
    unsigned ign2 : 12;
};

static_assert(8 == sizeof(page_table_entry), "page_table_entry size is incorrect");

/**
 *                      Page map level 1
 */
struct __attribute__((packed)) page_table {
    page_table_entry entries[512];
};

static_assert(4096 == sizeof(page_table), "page_table size is incorrect");