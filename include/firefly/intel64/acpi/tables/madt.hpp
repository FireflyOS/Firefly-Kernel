#pragma once

#include <frg/array.hpp>
#include <frg/tuple.hpp>
#include <frg/vector.hpp>

#include "firefly/compiler/compiler.hpp"
#include "firefly/intel64/acpi/acpi_table.hpp"
#include "firefly/logger.hpp"
#include "firefly/memory-manager/allocator.hpp"
#include "libk++/bits.h"
#include "libk++/fmt.hpp"

USED const frg::array<const char *, 6> madtInterruptDevices = {
    "local apic",
    "io apic",
    "io apic interrupt source override",
    "io apic NMI source",
    "io apic NMI",
    "local apic address override"
};

// Represents: MadtHeader::entryType
enum MadtEntryType : uint8_t {
    lApic,
    ioApic,
    sourceOverride,
    x2Apic = 9,
};

struct MadtHeader {
    uint8_t entryType;
    uint8_t recordLen;
} PACKED;

struct MadtEntryApic : MadtHeader {
    uint8_t processorId;
    uint8_t apicId;
    enum flags {
        // Check bit 0 first. If it's clear, check bit 1.
        // If neither of these bits are set, we should not
        // enable this CPU.
        processorEnabled = BIT(0),
        onlineCapable = BIT(1)
    };
} PACKED;

struct MadtEntryIoApic : MadtHeader {
    uint8_t ioApicId;
    uint8_t reserved;
    uint32_t ioApicAddress;
    uint64_t globalInterruptBase; /* Global system interrupt base */
} PACKED;

struct MadtSourceOverride : MadtHeader {
    uint8_t bus;     // constant, meaning ISA
    uint8_t source;  // Bus relative IRQ
    uint32_t gsi;    // what GSI this will issue
    uint16_t flags;
} PACKED;

/*
        Multiple Apic Description Table
        https://wiki.osdev.org/MADT
*/
struct AcpiMadt {
    AcpiSdt header;

    uint32_t localApicAddress;
    uint32_t ignore;  // Flags. Indicates whether a PIC is installed so
                      // that an os knows if it should disable it. We do it unconditionally.
    char entries[];

    // Find and return a tuple<vector> of every apic and io apic device reported by the MADT.
    using T = frg::tuple<frg::vector<MadtEntryApic *, Allocator> *, frg::vector<MadtEntryIoApic *, Allocator> *, frg::vector<MadtSourceOverride *, Allocator> *>;
    inline T enumerate() {
        frg::vector<MadtEntryApic *, Allocator> apics;
        frg::vector<MadtEntryIoApic *, Allocator> io_apics;
        frg::vector<MadtSourceOverride *, Allocator> source_overrides;

        // Madt entries range from  'madt_entries_start' to 'madt_entries_end'
        auto madt_entries_start = (uint8_t *)entries;
        auto madt_entries_end = reinterpret_cast<uintptr_t>(madt_entries_start) + header.length;

        // Iterate over all madt entries
        while (reinterpret_cast<uintptr_t>(madt_entries_start) < madt_entries_end) {
            MadtHeader *hdr = reinterpret_cast<MadtHeader *>(madt_entries_start);

            switch (hdr->entryType) {
                case MadtEntryType::lApic:
                    apics.push(reinterpret_cast<MadtEntryApic *>(madt_entries_start));
                    break;

                case MadtEntryType::ioApic:
                    io_apics.push(reinterpret_cast<MadtEntryIoApic *>(madt_entries_start));
                    break;

                case MadtEntryType::sourceOverride:
                    source_overrides.push(reinterpret_cast<MadtSourceOverride *>(madt_entries_start));
                    break;

                case MadtEntryType::x2Apic:
                    firefly::debugLine << "x2apic\n";
                    break;

                default:
                    break;
            }

            // Move on to the next entry
            madt_entries_start += hdr->recordLen;
        }

        firefly::logLine << "Found " << apics.size() << " APIC(s) and " << io_apics.size() << " IOAPIC(s)\n"
                         << firefly::fmt::endl;
        firefly::logLine << "Found " << source_overrides.size() << " source overrides\n";
        return { &apics, &io_apics, &source_overrides };
    }
} PACKED;
