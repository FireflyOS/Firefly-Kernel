#include "firefly/intel64/pit/pit.hpp"

#include <cstddef>
#include <cstdint>

#include "firefly/drivers/ports.hpp"
#include "firefly/intel64/int/interrupt.hpp"
#include "firefly/logger.hpp"

namespace {
constexpr const uint64_t PIT_FREQUENCY = 1000;
constexpr const uint64_t PIT_HZ = 1193180;
}  // namespace

namespace firefly::kernel {
namespace timer::pit {

void init() {
    const uint64_t divisor = PIT_HZ / PIT_FREQUENCY;

    io::outb(0x43, 0x36);
    io::outb(0x40, static_cast<uint8_t>(divisor));
    io::outb(0x40, static_cast<uint8_t>(divisor >> 8));
}

void destroy() {
}
}  // namespace timer::pit
}  // namespace firefly::kernel
