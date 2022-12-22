#pragma once

#include <cstddef>
#include <cstdint>

#include "firefly/compiler/compiler.hpp"
#include "firefly/intel64/page_permissions.hpp"
#include "firefly/logger.hpp"
#include "firefly/memory-manager/mm.hpp"
#include "firefly/memory-manager/page.hpp"
#include "firefly/memory-manager/primary/primary_phys.hpp"

namespace firefly::kernel::core::paging {

struct StructuredPte {
    uint8_t present;
    uint64_t address;
};

union Pte {
    uint64_t All;

    struct {
        uint8_t present : 1;
        uint8_t readwrite : 1;
        uint8_t supervisor : 1;
        uint8_t writethrough : 1;
        uint8_t cache_disabled : 1;
        uint8_t accessed : 1;
        uint8_t dirty : 1;
        uint8_t pagesize : 1;
        uint8_t global : 1;
        uint8_t ignore : 3;
        uint64_t address : 52;
    } fields PACKED;

    // A hack to use structured binding with a filter (only select a few elements)
    inline StructuredPte destructure() const {
        return { fields.present, fields.address };
    }

    inline void create(uint64_t addr, int flags, PageSize ps) {
        auto check_flags = [&](int flags, int bit) {
            return static_cast<uint8_t>((flags >> bit) & 1);
        };

        this->fields = {
            .present = check_flags(flags, 0),
            .readwrite = check_flags(flags, 1),
            .supervisor = check_flags(flags, 2),
            .writethrough = check_flags(flags, 3),
            .cache_disabled = check_flags(flags, 4),
            .accessed = 0,
            .dirty = 0,
            .pagesize = check_flags(flags, 7),  // PS/PAT bit
            .global = check_flags(flags, 8),
            .ignore = 0,
            .address = pagelist.pfn(addr)  // Page Frame Number
        };
    }
};

// Pml => Page map level
// (A vague term to help define Pml{1,2,3,4} or PDPT,PDP,PD,PT)
struct Pml {
    Pte entries[512] = {};

    // Retrieve the next pml, creating it if necessary.
    inline Pml* next(int depth, AccessFlags flags, PageSize ps) {
        const auto& pte = this->entries[depth];
        auto [present, address] = pte.destructure();

        if (present) {
            return reinterpret_cast<Pml*>(address << PAGE_SHIFT);
        }

        uint64_t addr = reinterpret_cast<uint64_t>(operator new(PageSize::Size4K));
        if (!addr) return nullptr;

        this->entries[depth].create(addr, static_cast<int>(flags), ps);
        return reinterpret_cast<Pml*>(addr);
    };

    inline PhysicalAddress operator new(size_t sz) {
        return mm::Physical::allocate(sz, FillMode::ZERO);
    }
} PACKED;
}  // namespace firefly::kernel::core::paging