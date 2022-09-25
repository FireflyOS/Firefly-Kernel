#pragma once

#include <stdint.h>

#define LAPIC_REG_ICR0 0x300
#define LAPIC_REG_ICR1 0x310
#define LAPIC_REG_EOI 0x0B0
#define LAPIC_REG_TIMER 0x320
#define LAPIC_REG_TIMER_CURRENT 0x390
#define LAPIC_REG_TIMER_DIV 0x3E0
#define LAPIC_REG_TIMER_INITIAL 0x380

namespace firefly::kernel::apic {
class Apic {
protected:
    uint64_t address;

public:
    Apic(uint64_t address) {
        this->address = address;
    }

    void setAddress(uint64_t address) {
        this->address = address;
    }

    void enable();
    void write(uint32_t offset, uint32_t value);
    uint32_t read(uint32_t offset) const;
    void clearErrors();
    void setIPIDest(uint32_t ap);
    void sendEOI();
};

void init();
}  // namespace firefly::kernel::apic
