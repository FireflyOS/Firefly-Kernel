#pragma once

#include <stddef.h>
#include <stdint.h>

#define BMP_BLOCK_SIZE 32

namespace firefly::libkern {
typedef struct
{
    uint32_t* pool;
} bitmap_t;

constexpr int BIT_SET = 1;
constexpr int BIT_CLEAR = 0;

typedef struct
{
    bool success;
    int64_t bit_offset; // Only used by get() & find_first()
    int64_t unpack() {
        if (success) return bit_offset;
        else return -1;
    }
} bitmap_res_t;

__attribute__((always_inline)) inline bitmap_res_t bitmap_ok(int64_t bit_offset = 0) {
    return { true, bit_offset };
}

__attribute__((always_inline)) inline bitmap_res_t bitmap_fail() {
    return { false, 0 };
}

class Bitmap {    
public:
    void init(bitmap_t*, size_t size);
    bitmap_res_t set(size_t bit);
    bitmap_res_t get(size_t bit);
    void clear(size_t bit);
    void print(size_t bit);
    void purge();
    void setall();
    int64_t find_first(int type);
    void resize(size_t size);
private:
    bitmap_t bitmap_instance;
    size_t limit;
};
}  // namespace firefly::libkern
