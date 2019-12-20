#include <drive.hpp>
#include <err.hpp>
#include <fat32.hpp>
#include <mmap.hpp>
#include <paging.hpp>

unsigned char *buffer = reinterpret_cast<unsigned char *>(0xfc00);

extern "C" void __attribute__((noreturn)) bmain() {
    read_mmap();

    if (drive::read(buffer, 0, 0, 1))
        err();

    fat32::mbr *mbr = reinterpret_cast<fat32::mbr *>(buffer);

    if (mbr->boot_sig != 0xaa55)
        err();
    else
        asm volatile(
            "int $0x10\n"
            :
            : "a"(0x0e00 | 'S'));

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
// payload