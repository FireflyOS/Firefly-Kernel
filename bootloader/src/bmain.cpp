#include <drive.hpp>
#include <fat32.hpp>  // abc
#include <mmap.hpp>

extern "C" void[[noreturn]] bmain() {
    read_mmap();

    if (drive::read(reinterpret_cast<void *>(0xfc00), 0, 0, 1))
        asm volatile(
            "int $0x10\n"
            :
            : "a"(0x0e00 | 'E'));
    else

        // load kernel at 1M
        // and modules after it
        // then give kernel pointer to list of memory ranges
        // and list of module descriptions

        asm volatile(
            "int $0x10\n"
            :
            : "a"(0x0e00 | 'S'));

    while (1)
        ;
}

// module files could probably simply have the format:
// 8 byte offset into payload of init function
// 8 byte offset into payload of destruct function
// payload