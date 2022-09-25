#pragma once

#include <cstdint>

namespace firefly::kernel {
namespace timer::pit {
void init();
void destroy();

uint64_t counter();
}  // namespace timer::pit
}  // namespace firefly::kernel
