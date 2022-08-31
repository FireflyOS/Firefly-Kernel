#include "libk++/memory.hpp"

void *memset(void *dest, int val, size_t len)
{
    uint8_t *ptr = (uint8_t *)dest;

    while (len--)
    {
        *ptr++ = val;
    }

    return dest;
}

void *memcpy(void *dest, const void *src, size_t len)
{
    uint8_t *_dest = (uint8_t *)dest;
    uint8_t *_src = (uint8_t *)src;

    for (; len != 0; len--)
    {
        *_dest = *_src;

        _dest++;
        _src++;
    }

    return dest;
}
