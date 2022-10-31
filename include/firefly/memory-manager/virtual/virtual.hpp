#pragma once

#include <stddef.h>
#include <stdint.h>

#include <frigg/frg/manual_box.hpp>

#include "firefly/logger.hpp"
#include "firefly/memory-manager/virtual/vspace.hpp"

namespace firefly::kernel::mm {

/* Represents kernel page tables. Global singleton. */
class kernelPageSpace : public VirtualSpace {
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
class userPageSpace : public VirtualSpace {
public:
    userPageSpace(PhysicalAddress root) {
        initSpace(root);
    }

    VIRTUAL_SPACE_FUNC_MAP;
    VIRTUAL_SPACE_FUNC_MAP_RANGE;
    VIRTUAL_SPACE_FUNC_UNMAP;
};

}  // namespace firefly::kernel::mm
