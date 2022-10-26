#pragma once

#include <cstdint>
#include <frg/manual_box.hpp>

#include "firefly/intel64/acpi/acpi_table.hpp"
#include "firefly/logger.hpp"

namespace firefly::kernel::timer {
constexpr const uint8_t GENERAL_CAPS_REG = 0x0;
constexpr const uint8_t GENERAL_CONF_REG = 0x10;
constexpr const uint8_t GENERAL_INT_STATUS_REG = 0x20;
constexpr const uint8_t MAIN_COUNTER_VALUE_REG = 0xF0;

constexpr uint16_t timer_config(uint8_t n) {
    return (0x100 + 0x20 * n);
}

constexpr uint16_t timer_comparator(uint8_t n) {
    return (0x108 + 0x20 * n);
}

constexpr uint16_t timer_fsb_int_route(uint8_t n) {
    return (0x110 + 0x20 * n);
}

class HPET {
private:
    friend class frg::manual_box<HPET>;

protected:
    uint64_t address;
    uint32_t period;
    uint32_t frequency;
    uint16_t minimumTicks;

    void write(const uint64_t reg, const uint64_t value);
    uint64_t read(const uint64_t reg) const;


public:
    HPET(AcpiAddress& acpiAddress, uint16_t minimumClockTicks) {
        this->address = acpiAddress.address;
        this->minimumTicks = minimumClockTicks;
    }

    static HPET& accessor();
    static void init();
    static void deinit();

    void initialize();
    void enable();
    void disable();
};
};  // namespace firefly::kernel::timer
