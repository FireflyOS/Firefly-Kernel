#pragma once

/**
 *                          the BIOS hard drive driver
 */
namespace drive {
    extern "C" unsigned short read(void *dest, unsigned short segment, unsigned short sector, unsigned short amt);
    extern "C" unsigned short write(void *src, unsigned short sector, unsigned char amount);
}  // namespace drive