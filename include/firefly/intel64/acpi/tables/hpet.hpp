#pragma once

#include <cstdint>

#include "firefly/compiler/compiler.hpp"
#include "firefly/intel64/acpi/acpi_table.hpp"

// https://www.intel.com/content/dam/www/public/us/en/documents/technical-specifications/software-developers-hpet-spec-1-0a.pdf
// page 30
struct AcpiHpet {
    AcpiSdt header;

    uint32_t hpetId;

    AcpiAddress address;

    const uint8_t hpetNumber;

    uint16_t minimumClockTicks;  // minimum clocks ticks without lost interruts
    struct {
        uint8_t pageProtection : 4;
        uint8_t oemAttributes : 4;
    };
} PACKED;
