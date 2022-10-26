#pragma once

#include <cstdint>
#include <frg/manual_box.hpp>

#include "firefly/intel64/acpi/acpi_table.hpp"
#include "firefly/logger.hpp"

namespace firefly::kernel::timer {
constexpr const uint64_t GENERAL_CAPS_REG = 0x0;
constexpr const uint64_t GENERAL_CONF_REG = 0x10;
constexpr const uint64_t GENERAL_INT_STATUS_REG = 0x20;
constexpr const uint64_t MAIN_COUNTER_VALUE_REG = 0xF0;

constexpr const uint64_t TIMER0_CONFIG_CAPS_REG = 0x100;
constexpr const uint64_t TIMER0_COMPARATOR_REG = 0x108;
constexpr const uint64_t TIMER0_FSB_INT_ROUTE_REG = 0x110;

constexpr const uint64_t TIMER1_CONFIG_CAPS_REG = 0x120;
constexpr const uint64_t TIMER1_COMPARATOR_REG = 0x128;
constexpr const uint64_t TIMER1_FSB_INT_ROUTE_REG = 0x130;

constexpr const uint64_t TIMER2_CONFIG_CAPS_REG = 0x140;
constexpr const uint64_t TIMER2_COMPARATOR_REG = 0x148;
constexpr const uint64_t TIMER2_FSB_INT_ROUTE_REG = 0x150;


class HPET {
private:
    friend class frg::manual_box<HPET>;

protected:
    uint64_t address;

    void write(const uint64_t reg, const uint64_t value);
    uint64_t read(const uint64_t reg) const;


public:
    HPET(AcpiAddress& acpiAddress) {
        this->address = acpiAddress.address;
    }

    static HPET& accessor();
    static void init();
    static void deinit();
};
};  // namespace firefly::kernel::timer
