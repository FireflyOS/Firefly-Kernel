#include "firefly/intel64/acpi/acpi.hpp"

#include <cstdlib/cassert.h>

#include "firefly/limine.hpp"
#include "firefly/logger.hpp"
#include "firefly/panic.hpp"

namespace firefly::kernel::core::acpi {

struct limine_rsdp_request rsdp_request = {
    .id = LIMINE_RSDP_REQUEST,
    .revision = 0,
    .response = nullptr
};

frg::manual_box<Acpi> acpiSingleton;

Acpi& Acpi::accessor() {
    return *acpiSingleton;
}

void Acpi::init() {
    acpiSingleton.initialize();
    auto const rsdp_response = rsdp_request.response;

    if (unlikely(!rsdp_response || !rsdp_response->address))
        firefly::panic("Unable to obtain basic ACPI information");

    // Verify the RSDP checksum
    auto& rsdp = acpiSingleton.get()->rsdp;
    rsdp = *reinterpret_cast<AcpiRsdp*>(rsdp_response->address);
    rsdp.validateOrPanic(reinterpret_cast<uint8_t*>(rsdp_response->address));

	// Initialize the RSDT or XSDT if available
    const auto& self = acpiSingleton.get();
    self->useXSDT = rsdp.revision >= 2;
    self->divisor = self->useXSDT ? 8 : 4;

    if (self->useXSDT)
        self->xsdt.root = reinterpret_cast<decltype(self->xsdt.root)>(rsdp.xsdtAddress);
    else
        self->rsdt.root = reinterpret_cast<decltype(self->rsdt.root)>(rsdp.rsdtAddress);

    self->rootHeader = self->useXSDT ? self->xsdt.root->header : self->rsdt.root->header;
    self->entries = (self->rootHeader.length - sizeof(self->rootHeader)) / self->divisor;

    info_logger << "Initialized: ACPI\n";
}

void Acpi::dumpTables() const {
	info_logger << "Using " << (useXSDT ? "xsdt" : "rsdt") << ", number of acpi tables: " << entries << '\n';

	for (int i = 0; i < entries; i++) {
		AcpiSdt* sdt = reinterpret_cast<AcpiSdt*>((useXSDT ? xsdt.root->tables[i] : rsdt.root->tables[i]));

		// The SDT descriptors are not null-terminated.
		// They are, however always 4 characters long, so we can just iterate over them like this.
		for (int j = 0; j < 4; j++) {
			info_logger << sdt->signature[j];
		}
		info_logger << '\n';
	}
}

AcpiTable Acpi::find(frg::string_view identifier) const {
    assert_truth(identifier.size() == 4 && "Acpi identifier must be 4 characters!");

    for (int i = 0; i < entries; i++) {
        AcpiSdt* sdt = reinterpret_cast<AcpiSdt*>((useXSDT ? xsdt.root->tables[i] : rsdt.root->tables[i]));

        if (sdt->compareSignature(identifier.data()) && sdt->validate()) {
            return AcpiTable(sdt);
        }
    }

    info_logger << "Couldn't find table: " << identifier.data() << '\n';
    return nullptr;
}

}  // namespace firefly::kernel::core::acpi
