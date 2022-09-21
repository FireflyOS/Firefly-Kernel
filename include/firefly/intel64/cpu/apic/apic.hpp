#pragma once

#include <stdint.h>

namespace firefly::kernel::apic {
class Apic {
protected:
    uint64_t address;

public:
    Apic(uint64_t address) {
        this->address = address;
    }

    void enable();
    void write(uint32_t offset, uint32_t value);
    uint32_t read(uint32_t offset) const;
};

void init();
}  // namespace firefly::kernel::apic
