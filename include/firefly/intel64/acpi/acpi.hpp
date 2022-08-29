#pragma once

#include <cstdint>
#include <frigg/frg/manual_box.hpp>

#include "acpi_table.hpp"
#include "firefly/compiler/clang++.hpp"

namespace firefly::kernel::core::acpi {

// RSDP version 1.0
struct RSDP {
    char signature[8];
    uint8_t checksum;
    char oemid[6];
    uint8_t revision;
    uint32_t rsdtAddress;
} PACKED;

// RSDP version 2+
struct RSDPextension : RSDP {
    uint32_t length;
    uint64_t xsdtAddress;
    uint8_t extendedChecksum;
    uint8_t reserved[3];
} PACKED;

class Acpi {
private:
    friend class frg::manual_box<Acpi>;
    Acpi() = default;

private:
    AcpiTable<uint32_t> rsdt;
    AcpiTable<uint64_t> xsdt;
};

}  // namespace firefly::kernel::core::acpi