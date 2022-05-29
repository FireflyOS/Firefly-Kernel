#include "libk++/bitmap.h"

#include <algorithm>
#include <cstdlib/cstring.h>

#include "firefly/logger.hpp"

namespace firefly::libkern {
void Bitmap::init(uint32_t *bitmap, size_t size) {
    this->bitmap_instance = bitmap;
    this->limit = size;
    this->purge();
}

bitmap_res_t Bitmap::set(uint32_t bit) {
    if (bit > this->bitmap_instance[this->limit / BMP_BLOCK_SIZE] || bit < 0) return bitmap_fail();
    return bitmap_ok(this->bitmap_instance[bit / BMP_BLOCK_SIZE] |= (1 << (bit % BMP_BLOCK_SIZE)));
}

bitmap_res_t Bitmap::get(uint32_t bit) {
    if (bit > this->bitmap_instance[this->limit / BMP_BLOCK_SIZE] || bit < 0) return bitmap_fail();
    return bitmap_ok(this->bitmap_instance[bit / BMP_BLOCK_SIZE] >> bit % BMP_BLOCK_SIZE & 1);
}

void Bitmap::print(uint32_t bit) {
    size_t result = (this->bitmap_instance[bit / BMP_BLOCK_SIZE] >> bit % BMP_BLOCK_SIZE) & 0x1;
    kernel::info_logger << "bitmap[" << bit << "] = " << result << kernel::logger::endl;
}

bitmap_res_t Bitmap::clear(uint32_t bit) {
    if (bit > this->bitmap_instance[this->limit / BMP_BLOCK_SIZE] || bit < 0) return bitmap_fail();
    return bitmap_ok(this->bitmap_instance[bit / BMP_BLOCK_SIZE] &= ~(1 << (bit % BMP_BLOCK_SIZE)));
}

void Bitmap::purge() {
    for (size_t i = 0; i < this->limit; i++) {
        this->clear(i);
    }
}

void Bitmap::setall() {
    memset(this->bitmap_instance, 0xFF, this->limit);
}

int64_t Bitmap::find_first(int type) {
    int bitmap_type = (type == BIT_SET) ? BIT_SET : BIT_CLEAR;
    for (int64_t i = 0; i < (int64_t)this->limit; i++) {
        // No need to check for errors from unpack() as errors are caused
        // if we go out of bounds regarding the bitmap which clearly isn't possible here
        if (this->get(i).unpack() != bitmap_type) {
            return i;
        }
    }

    return -1;
}

void Bitmap::resize(size_t size) {
    if (this->limit == size) return;

    // Zero out the unused bytes so that they can be free of garbage
    // if the memory is released or the bitmap instance resized (to something larger)
    if (size < this->limit) {
        for (size_t i = size; i < this->limit; i++) {
            this->clear(i);
        }
    }

    this->limit = size;
}

int64_t Bitmap::allocator_conversion(bool bit_to_address, size_t bit_or_address) {
    // Bit to page aligned address
    if (bit_to_address)
        return bit_or_address * 0x1000;

    // Page aligned address to bit
    return bit_or_address / 0x1000;
}

}  // namespace firefly::libkern
