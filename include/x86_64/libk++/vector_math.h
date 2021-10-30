#pragma once

#include <stdint.h>

namespace firefly::libkern {
typedef struct Vector2
{
    uint64_t x;
    uint64_t y;
    Vector2 Empty() { return {0,0}; }
} Vec2;
}  // namespace firefly::libkern
