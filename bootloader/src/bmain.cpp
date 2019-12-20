#include <drive.hpp>
#include <err.hpp>
#include <fat32.hpp>
#include <mmap.hpp>
#include <paging.hpp>
#include <stdio.hpp>

unsigned char *buffer = reinterpret_cast<unsigned char *>(0xfc00);

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

    while (1)
        ;
}

// module files could probably simply have the format:
// 8 byte offset into payload of init function
// 8 byte offset into payload of destruct function
// payloadz