#pragma once

/**
 *                          the BIOS hard drive driver
 */
namespace drive {
    extern "C" unsigned short read(void *dest, unsigned int segment, unsigned int sector, unsigned int amt);
    extern "C" unsigned short write(void *src, unsigned int sector, unsigned char amount);
}  // namespace drive