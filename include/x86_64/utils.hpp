#pragma once

namespace firefly::kernel {
//Converts a single (B,G,R)(array) color pair into an RGB value (uint32_t)
uint32_t bgr2rgb(uint32_t bytes[], int offset);

uint32_t rev32(uint32_t bytes);
}  // namespace firefly::kernel