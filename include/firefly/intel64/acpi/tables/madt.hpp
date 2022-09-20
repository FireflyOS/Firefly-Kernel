#pragma once

#include <frg/array.hpp>
#include <frg/tuple.hpp>

#include "firefly/compiler/compiler.hpp"
#include "firefly/intel64/acpi/acpi_table.hpp"
#include "firefly/logger.hpp"
#include "libk++/bits.h"

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
    x2Apic = 9
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

    // Find and return a tuple<array> of every apic and io apic device reported by the MADT.
    // Todo: We need to use a vector for this, but we have no slab (yet).
    // For now we'll just hardcode '4', so a max of 4 cpus are supported.
    using T = frg::tuple<frg::array<MadtEntryApic *, 4>, frg::array<MadtEntryIoApic *, 4>>;
    inline T enumerate() const {
        frg::array<MadtEntryApic *, 4> apics{};
        frg::array<MadtEntryIoApic *, 4> io_apics{};
        int ioapic_idx{ 0 }, apic_idx{ 0 };  // Again, this will be replaced by a vector

        // Madt entries range from  'madt_entries_start' to 'madt_entries_end'
        auto madt_entries_start = (uint8_t *)entries;
        auto madt_entries_end = reinterpret_cast<uintptr_t>(madt_entries_start) + header.length;

        // Iterate over all madt entries
        while (reinterpret_cast<uintptr_t>(madt_entries_start) < madt_entries_end) {
            MadtHeader *hdr = reinterpret_cast<MadtHeader *>(madt_entries_start);

            switch (hdr->entryType) {
                case MadtEntryType::lApic:
                    apics[apic_idx++] = reinterpret_cast<MadtEntryApic *>(madt_entries_start);
                    break;

                case MadtEntryType::ioApic:
                    io_apics[ioapic_idx++] = reinterpret_cast<MadtEntryIoApic *>(madt_entries_start);
                    break;

                case MadtEntryType::x2Apic:
                    firefly::kernel::info_logger << "x2apic\n";
                    break;

                default:
                    break;
            }

            // Move on to the next entry
            madt_entries_start += hdr->recordLen;
        }

        return { apics, io_apics };
    }
} PACKED;