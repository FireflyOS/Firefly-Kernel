#pragma once

#include <stddef.h>
#include <stdint.h>

#define BMP_BLOCK_SIZE 8

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
    void init(uint32_t *bitmap, size_t size);
    bitmap_res_t set(uint32_t bit);
    bitmap_res_t get(uint32_t bit);
    bitmap_res_t clear(uint32_t bit);
    void print(uint32_t bit);
    void purge();
    void setall();
    int64_t find_first(int type);
    void resize(size_t size);
    int64_t allocator_conversion(bool bit_to_address, size_t bit_or_address);
private:
    uint32_t *bitmap_instance;
    size_t limit;
};
}  // namespace firefly::libkern
