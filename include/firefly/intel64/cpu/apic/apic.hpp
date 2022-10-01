#pragma once

#include <stdint.h>
#include "firefly/intel64/acpi/acpi.hpp"

#define IOAPIC_DEST(x) (((x)&0xFF)<<56)

namespace firefly::kernel::apic {

constexpr const uint8_t LVT_BASE = 0x20;

constexpr const uint32_t APIC_LVR = 0x30;
constexpr const uint32_t APIC_TASK_PRIORITY = 0x80;
constexpr const uint32_t APIC_ICR0 = 0x300;
constexpr const uint32_t APIC_ICR1 = 0x310;
constexpr const uint32_t APIC_LVTT = 0x320;     // Timer
constexpr const uint32_t APIC_LVTTHMR = 0x330;  // Thermal monitor
constexpr const uint32_t APIC_LVTPC = 0x340;    // Performance monitor
constexpr const uint32_t APIC_LVT0 = 0x350;     // Local INT #0
constexpr const uint32_t APIC_LVT1 = 0x360;     // Local INT #1
constexpr const uint32_t APIC_LVTERR = 0x370;   // Error
constexpr const uint32_t APIC_TIMER_INITIAL = 0x390;
constexpr const uint32_t APIC_TIMER_CURRENT = 0x390;
constexpr const uint32_t APIC_TIMER_DIVIDER = 0x3E0;
constexpr const uint32_t APIC_EOI = 0x0B0;
constexpr const uint32_t APIC_SPURIOUS = 0xF0;

constexpr const uint32_t APIC_MASKED = 0x10000;

constexpr const uint32_t IOAPIC_MASKED = BIT(16);
constexpr const uint32_t IOAPIC_LEVEL = BIT(15);
constexpr const uint32_t IOAPIC_INTPOL = BIT(13);
constexpr const uint32_t IOAPIC_DESTMODE = BIT(11);
constexpr const uint32_t IOAPIC_DELMODE_FIX = ((0)<<8);

constexpr const uint32_t IOAPIC_REDTBL_BASE = 0x10;
constexpr const uint32_t IOAPIC_REDTBL0 = 0x10;
constexpr const uint32_t IOAPIC_REDTBL1 = 0x12;


class Apic {
protected:
    uint64_t address;

public:
    Apic(uint64_t address) {
        this->address = address;
    }

    static void init();

    void setAddress(uint64_t address) {
        this->address = address;
    }

    void enable();
    void enableIRQ(uint8_t irq);
    void write(uint32_t offset, uint32_t value);
    uint32_t read(uint32_t offset) const;
    void clearErrors();
    void setIPIDest(uint32_t ap);
    void sendEOI();
};

class IOApic {
private:
    uint64_t address;

public:
    IOApic(uint64_t address) {
        this->address = address;
    }

    // TODO: Proper IOApic code
    static void initAll() {
	    using core::acpi::Acpi;
	    auto const& madt = reinterpret_cast<AcpiMadt*>(Acpi::accessor().mustFind("APIC"));
	    auto const ioapics = madt->enumerate().get<1>();
	    for (uint32_t i = 0; i < ioapics.size(); i++) {
		    auto ioapic = IOApic(ioapics[i]->ioApicAddress);
		    ioapic.init();
		    ioapic.enableIRQ(0);
		    ioapic.enableIRQ(1);
	    }
    }

    enum DeliveryMode {
        Edge = 0,
        Level = 1,
    };

    enum DestinationMode {
        Physical = 0,
        Logical = 1
    };

    void write(uint32_t offset, uint32_t value);
    uint32_t read(uint32_t offset) const;
    void init();
    void enableIRQ(uint8_t irq);
};

enum ApicTimerMode {
    OneShot = 0,
    Periodic = 1,
    TscDeadline = 2
};

}  // namespace firefly::kernel::apic
