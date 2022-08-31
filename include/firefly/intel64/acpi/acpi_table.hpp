#pragma once

#include <cstdint>

#include "firefly/compiler/compiler.hpp"
#include "libk++/fmt.hpp"

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

    inline bool validate() const {
        uint8_t sum = 0;
        for (uint32_t i = 0; i < this->length; i++)
            sum += reinterpret_cast<uint8_t *>(const_cast<AcpiSdt *>(this))[i];

        return sum == 0;
    }

    inline bool compareSignature(const char *other) const {
        return (firefly::libkern::fmt::strncmp(signature, other, 4)) == 0;
    }
} PACKED;

template <typename T>
class AcpiRootTable {
public:
    struct PACKED {
        AcpiSdt header;
        T tables[];  // Pointers to acpi tables
    } * root;
} PACKED;