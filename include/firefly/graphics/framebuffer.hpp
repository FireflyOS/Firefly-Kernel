#pragma once

#include <cstdint>

#include "firefly/compiler/compiler.hpp"
#include "firefly/memory-manager/mm.hpp"

namespace firefly::kernel {
class FramebufferDevice {
public:
    void init(PhysicalAddress fb, uint64_t width, uint64_t height, uint64_t pitch) {
        this->fb = static_cast<uint32_t *>(fb);
        this->pitch = pitch;
        this->height = height;
        this->width = width;
    }

private:
    uint32_t *fb;
    uint64_t width, height, pitch;
};

extern FramebufferDevice fbDev;
}  // namespace firefly::kernel
