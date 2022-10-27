#include "firefly/intel64/pit/pit.hpp"

#include <cstddef>
#include <cstdint>

#include "firefly/drivers/ports.hpp"
#include "firefly/intel64/cpu/apic/apic.hpp"
#include "firefly/intel64/int/interrupt.hpp"
#include "firefly/logger.hpp"

namespace {
constexpr const uint64_t PIT_HZ = 1193180;
constexpr const uint64_t PIT_FREQUENCY = 1000;  // 1ms or smth
}  // namespace

namespace firefly::kernel {
namespace timer::pit {

static volatile uint32_t count = 0;

static void timer_callback() {
    if (count == 0) return;
    count = count - 1;
}

void sleep(uint32_t ms) {
    count = ms;
    while (count > 0) {
        asm volatile("hlt");
    }
}

void init() {
    core::interrupt::registerIRQHandler(timer_callback, apic::IOApic::getGSI(0));
    apic::enableIRQ(0);

    const uint32_t divisor = PIT_HZ / PIT_FREQUENCY;

    // send the command byte
    io::outb(0x43, 0x36);

    // divisor needs to be sent bytewise
    uint8_t l = static_cast<uint8_t>(divisor & 0xFF);
    uint8_t h = static_cast<uint8_t>((divisor >> 8) & 0xFF);

    io::outb(0x40, l);
    io::outb(0x40, h);
}

void destroy() {
    core::interrupt::unregisterIRQHandler(apic::IOApic::getGSI(0));
}
}  // namespace timer::pit
}  // namespace firefly::kernel
