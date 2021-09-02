#include "x86_64/libk++/bitmap.h"
#include <stl/cstdlib/stdio.h>

namespace firefly::libkern
{
    Bitmap::Bitmap(bitmap_t *bitmap, size_t size)
    {
        this->bitmap_instance = *bitmap;
        this->limit = size;
        this->purge();
    }

    void Bitmap::set(size_t bit)
    {
        this->bitmap_instance.pool[bit / BMP_BLOCK_SIZE] |= (1 << (bit % BMP_BLOCK_SIZE));
    }

    bool Bitmap::get(size_t bit)
    {
        return this->bitmap_instance.pool[bit/BMP_BLOCK_SIZE];
    }

    void Bitmap::print(size_t bit)
    {
        size_t result = (this->bitmap_instance.pool[bit / BMP_BLOCK_SIZE] >> bit % BMP_BLOCK_SIZE) & 0x1;
        printf("bitmap[%d] = %d\n", bit, result);
    }

    void Bitmap::clear(size_t bit)
    {
        this->bitmap_instance.pool[bit / BMP_BLOCK_SIZE] &= ~(1 << (bit % BMP_BLOCK_SIZE));
    }

    void Bitmap::purge()
    {
        for (size_t i = 0; i < this->limit; i++)
        {
            this->clear(i);
        }
    }

} // namespace firefly::libkern
