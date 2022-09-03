#pragma once

#include <cstdint>
#include <frigg/frg/manual_box.hpp>
#include <frigg/frg/string.hpp>

#include "firefly/compiler/compiler.hpp"
#include "firefly/intel64/acpi/acpi_rsdp.hpp"
#include "firefly/intel64/acpi/acpi_table.hpp"
#include "firefly/intel64/acpi/tables/all.hpp"

namespace firefly::kernel::core::acpi {
using AcpiTable = void*;

class Acpi {
private:
    friend class frg::manual_box<Acpi>;
    Acpi() = default;

public:
    static void init();
    static Acpi& accessor();

    AcpiTable find(frg::string_view identifier) const;
    void dumpTables() const;

private:
    AcpiRsdp rsdp;
    AcpiSdt rootHeader;
    AcpiRootTable<uint32_t> rsdt;
    AcpiRootTable<uint64_t> xsdt;

    bool useXSDT{ false };
    int8_t divisor{ 0 };
    int entries{ 0 };
};
}  // namespace firefly::kernel::core::acpi