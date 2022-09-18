#pragma once

#include <stddef.h>
#include <stdint.h>

#include <frigg/frg/manual_box.hpp>

#include "firefly/logger.hpp"
#include "firefly/memory-manager/mm.hpp"
#include "firefly/memory-manager/virtual/vspace.hpp"

namespace firefly::kernel::mm {

/* Represents kernel page tables. Global singleton. */
class kernelPageSpace : VirtualSpace {
private:
    friend class frg::manual_box<kernelPageSpace>;
    kernelPageSpace(PhysicalAddress root) {
        initSpace(root);
    }

public:
    static void init();
    static kernelPageSpace &accessor();

    VIRTUAL_SPACE_FUNC_MAP_RANGE;
    VIRTUAL_SPACE_FUNC_UNMAP;
    VIRTUAL_SPACE_FUNC_MAP;
};

/* Represents user processes page tables. Private, one (or more) per task. Currently unused (no userspace) */
class userPageSpace : VirtualSpace {
    userPageSpace(PhysicalAddress root) {
        initSpace(root);
    }

    void map(T virtual_addr, T physical_addr, AccessFlags flags, PageSize page_size) const override {
        (void)virtual_addr;
        (void)physical_addr;
        (void)flags;
	(void)page_size;
        ConsoleLogger::log() << "userPageSpace: map() is a stub!\n";
    }

    void unmap(T virtual_addr) const override {
        (void)virtual_addr;
        ConsoleLogger::log() << "userPageSpace: map() is a stub!\n";
    }
};

}  // namespace firefly::kernel::mm
