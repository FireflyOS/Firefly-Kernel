#include "firefly/intel64/pit/pit.hpp"

#include <cstddef>
#include <cstdint>

#include "firefly/drivers/ports.hpp"
#include "firefly/intel64/cpu/apic/apic.hpp"
#include "firefly/intel64/int/interrupt.hpp"
#include "firefly/logger.hpp"
#include "firefly/timer/timer.hpp"

namespace {
constexpr const uint64_t PIT_HZ = 1193180;
constexpr const uint64_t PIT_FREQUENCY = 1000;  // 1ms or smth
}  // namespace

namespace firefly::kernel {
namespace timer::pit {

static void timer_callback() {
    timer::tick();
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

    timer::setFrequency(PIT_FREQUENCY);
}

void destroy() {
    core::interrupt::unregisterIRQHandler(apic::IOApic::getGSI(0));
}
}  // namespace timer::pit
}  // namespace firefly::kernel
