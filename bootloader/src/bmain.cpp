#include <drive.hpp>
#include <err.hpp>
#include <fat32.hpp>
#include <handover.hpp>
#include <mmap.hpp>
#include <paging.hpp>
#include <stdio.hpp>

/**
 *                          Bootloader main entry point.
 */
extern "C" void __attribute__((noreturn)) bmain() {
    read_mmap();
    unsigned long binary_size = fat32::loadfs();

    if (!binary_size)
        err("failed to read kernel");

    setup_pages(binary_size);

    // load kernel at 1M
    // and modules after it
    // then give kernel pointer to list of memory ranges
    // and list of module descriptions

    printf("done\r\n");

    handover();

    while (true)
        ;
}

// module files could probably simply have the format:
// 8 byte offset into payload of init function
// 8 byte offset into payload of destruct function
// payloadz