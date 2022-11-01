#pragma once

#include <cstddef>
#include <cstdint>

#include "firefly/compiler/compiler.hpp"
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
    uint64_t* cr3;

    uint64_t* stackBottom;
    uint64_t* stack;

    iframe regs;

    frg::string_view descriptor;

    Task(frg::string_view _descriptor) {
        cr3 = core::paging::allocatePageTable();
        descriptor = _descriptor;
        stackBottom = reinterpret_cast<uint64_t*>(mm::Physical::must_allocate(PageSize::Size4K * 2));
        stack = stackBottom + PageSize::Size4K * 2 - 1;
    }

    void switchPageMap() {
        mm::writeCR3(reinterpret_cast<uintptr_t>(cr3));
    }
};

extern "C" {
extern void _switchTask(int64_t rsp, int64_t rip);
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
    void preempt();
};


}  // namespace firefly::kernel::scheduler
