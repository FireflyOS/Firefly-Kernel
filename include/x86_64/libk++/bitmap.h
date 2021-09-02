#pragma once

#include <stddef.h>
#include <stdint.h>

#define BMP_BLOCK_SIZE sizeof(uint32_t) * 8

namespace firefly::libkern {
typedef struct
{
    uint32_t* pool;
} bitmap_t;

class Bitmap {
public:
    Bitmap(bitmap_t*, size_t size);
    void set(size_t bit);
    bool get(size_t bit);
    void clear(size_t bit);
    void print(size_t bit);
    void purge();
private:
    bitmap_t bitmap_instance;
    size_t limit;
};
}  // namespace firefly::libkern
