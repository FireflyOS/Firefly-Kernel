#pragma once

#include <cstdint>

#include "firefly/compiler/compiler.hpp"
#include "firefly/logger.hpp"
#include "libk++/cstring.hpp"

#define ACPI_LOOP_AND_LOG(info, max, str) \
    firefly::logLine << info;             \
    for (int i = 0; i < max; i++)         \
        firefly::logLine << str[i];       \
    firefly::logLine << '\n'              \
                     << firefly::fmt::endl;

struct AcpiSdt {
    char signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char oemString[6];
    char oemTableId[8];
    uint32_t oemRevision;
    char creatorId[4];
    uint32_t creatorRevision;

    inline void logInfo() const {
        ACPI_LOOP_AND_LOG("Signature: ", 4, signature);
        ACPI_LOOP_AND_LOG("OEM: ", 6, oemString);
        ACPI_LOOP_AND_LOG("OEM-ID: ", 8, oemTableId);
    }

    inline bool validate() const {
        uint8_t sum = 0;
        for (uint32_t i = 0; i < this->length; i++)
            sum += reinterpret_cast<uint8_t *>(const_cast<AcpiSdt *>(this))[i];

        return sum == 0;
    }

    inline bool compareSignature(const char *other) const {
        using firefly::libkern::cstring::strncmp;
        return (strncmp(signature, other, 4)) == 0;
    }
} PACKED;

struct AcpiAddress {
    uint8_t addressSpaceID;  // 0 - System Memory
                             // 1 - System I/O
    uint8_t registerBitWidth;
    uint8_t registerBitOffset;
    uint8_t reserved;
    uint64_t address;
} PACKED;

template <typename T>
class AcpiRootTable {
public:
    struct PACKED {
        AcpiSdt header;
        T tables[];  // Pointers to acpi tables
    } * root;
} PACKED;
