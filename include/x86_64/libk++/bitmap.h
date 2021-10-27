#pragma once

#include <stddef.h>
#include <stdint.h>

#define BMP_BLOCK_SIZE 32

namespace firefly::libkern {
typedef struct
{
    uint32_t* pool;
} bitmap_t;

class Bitmap {
public:
    void init(bitmap_t*, size_t size);
    void set(size_t bit);
    bool get(size_t bit);
    void clear(size_t bit);
    void print(size_t bit);
    void purge();
    void setall();
private:
    bitmap_t bitmap_instance;
    size_t limit;
};
}  // namespace firefly::libkern
