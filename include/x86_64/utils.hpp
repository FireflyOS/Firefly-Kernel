#pragma once

namespace firefly::kernel {
uint32_t bgr2rgb(uint32_t bytes[], int offset);
uint32_t rev32(uint32_t bytes);
}  // namespace firefly::kernel