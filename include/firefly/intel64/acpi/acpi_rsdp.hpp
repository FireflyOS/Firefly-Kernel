#pragma once

#include <cstdint>

#include "firefly/compiler/compiler.hpp"
#include "firefly/panic.hpp"

namespace firefly::kernel::core::acpi {
struct AcpiRsdp {
    // RSDP version 1.0
    char signature[8];
    uint8_t checksum;
    char oemString[6];
    uint8_t revision;
    uint32_t rsdtAddress;

    // RSDP version 2+
    uint32_t length;
    uint64_t xsdtAddress;
    uint8_t extendedChecksum;
    uint8_t reserved[3];

    inline void validateOrPanic(uint8_t *ptr) {
        int checksum{ 0 };
        for (uint64_t i = 0; i < length; i++)
            checksum += *ptr++;

        if ((checksum & 0xff) != 0)
            firefly::panic("Bad RSDP checksum");
    }
} PACKED;
}  // namespace firefly::kernel::core::acpi