#pragma once

#include "firefly/intel64/acpi/acpi_table.hpp"

/*
        Boot Graphics Resource Table
        https://wiki.osdev.org/BGRT
*/
struct AcpiBgrt {
    /* SDT Header */
    AcpiSdt header;

    /* BGRT contents */
    uint16_t version;
    uint8_t status;
    uint8_t imageType;
    uint64_t imageAddress;
    uint32_t xOffset;
    uint32_t yOffset;

    inline bool valid() const {
        return version == 1 && status == 0 && (imageType & 1) == 0;
    }
} PACKED;