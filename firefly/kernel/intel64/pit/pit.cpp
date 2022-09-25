#include <cstdint>
#include <cstddef>
#include "firefly/intel64/int/interrupt.hpp"
#include "firefly/intel64/pit/pit.hpp"
#include "firefly/drivers/ports.hpp"
#include "firefly/logger.hpp"

namespace {
	constexpr const uint64_t PIT_FREQUENCY = 1000;
	constexpr const uint64_t PIT_HZ = 1193180;
	size_t pit_counter = 0;
}

namespace firefly::kernel {
namespace timer::pit {

void init() {
	uint64_t divisor = PIT_HZ / PIT_FREQUENCY;

	io::outb(0x43, 0x36);
	io::outb(0x40, static_cast<uint8_t>(divisor));
	io::outb(0x40, static_cast<uint8_t>(divisor >> 8));

}
void destroy() {

}
uint64_t counter() {
	return pit_counter;
}
}  // namespace timer::pit
}  // namespace firefly::kernel
