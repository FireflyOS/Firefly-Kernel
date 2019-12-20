#pragma once

/**
 *                          the BIOS hard drive driver
 */
namespace drive {
    extern "C" bool read(void *dest, unsigned short segment, unsigned short sector, unsigned short amt);
    extern "C" bool write(void *src, unsigned short sector, unsigned char amount);
}  // namespace drive