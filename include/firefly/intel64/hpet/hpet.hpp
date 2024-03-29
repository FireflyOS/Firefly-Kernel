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
    friend class HPETTimer;

protected:
    uint64_t address;
    uint32_t period;
    uint32_t frequency;
    uint16_t minimumTicks;

    void write(const uint64_t reg, const uint64_t value);
    uint64_t read(const uint64_t reg) const;

    inline uint64_t counter() const {
        return read(MAIN_COUNTER_VALUE_REG);
    }

    inline uint64_t usecToTicks(const uint64_t usec) const {
        const uint64_t femtosecond = 1000000000;
        const uint64_t ticks = read(MAIN_COUNTER_VALUE_REG);
        return ticks + (usec * femtosecond) / period;
    }


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

    inline void usleep(uint64_t usec) const {
        const uint64_t required_hpet_ticks = usecToTicks(usec);
        while (counter() < required_hpet_ticks)
            asm volatile("pause" ::
                             : "memory");
    }
};

class HPETTimer {
private:
    bool periodicSupport;
    uint8_t timerNum;

public:
    HPETTimer(bool periodicSupport, uint8_t timerNum) {
        this->periodicSupport = periodicSupport;
        this->timerNum = timerNum;
    }

    void enable();
    void disable();
    void setIoApicOutput(uint8_t output);
    void setOneshotMode();
    void setPeriodicMode();
};
};  // namespace firefly::kernel::timer
