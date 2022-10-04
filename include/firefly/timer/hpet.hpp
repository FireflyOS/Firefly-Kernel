#pragma once

#include <cstdint>

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
public:
    static void init();
    static void deinit();
};
};  // namespace firefly::kernel::timer
