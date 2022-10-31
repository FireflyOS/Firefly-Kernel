#pragma once

#include <cstddef>
#include <cstdint>

#include "firefly/intel64/int/interrupt.hpp"
#include "firefly/intel64/paging.hpp"
#include "firefly/memory-manager/allocator.hpp"
#include "firefly/memory-manager/primary/primary_phys.hpp"
#include "firefly/memory-manager/virtual/virtual.hpp"
#include "firefly/memory-manager/virtual/vspace.hpp"
#include "frg/string.hpp"
#include "frg/vector.hpp"

namespace firefly::kernel::scheduler {
using core::interrupt::iframe;

struct Task {
    iframe regs;
    frg::string_view descriptor;
    uint64_t* cr3;
    uint64_t* stack;

    Task(frg::string_view _descriptor) {
        cr3 = core::paging::allocatePageTable();
        descriptor = _descriptor;
        stack = 0;
    }

    void switchPageMap() {
        mm::writeCR3(reinterpret_cast<uintptr_t>(cr3));
    }
};

class Scheduler {
protected:
    frg::vector<Task, Allocator> tasks;
    uint64_t currTask;

public:
    static void init();
    static Scheduler& accessor();
    void reschedule(iframe regs);
    void registerTask(Task task);
};

void switchContext(Task* task);

}  // namespace firefly::kernel::scheduler
