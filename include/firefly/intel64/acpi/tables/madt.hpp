#pragma once

#include "firefly/compiler/compiler.hpp"
#include "firefly/intel64/acpi/acpi_table.hpp"

/*
        Multiple Apic Description Table
        https://wiki.osdev.org/MADT
*/
struct AcpiMadt {
	AcpiSdt header;

	
} PACKED;