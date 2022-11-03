#pragma once

#include <cstddef>
#include <cstdint>

#include "firefly/memory-manager/allocator.hpp"
#include "firefly/scheduler/process.hpp"
#include "firefly/scheduler/thread.hpp"
#include "frg/vector.hpp"

namespace firefly::kernel::scheduler {
class Scheduler {
protected:
    frg::vector<Process*, Allocator> procs;
    // use some sort of queue ig?
    frg::vector<Thread*, Allocator> threads;
    uint64_t currTask;

public:
    static void init();
    static Scheduler& accessor();

    void yield();
    void doSwitch();
    void schedule(RegisterContext* regs);

    void registerProcess(Process* process);
    void insertThread(Thread* thread);
};


}  // namespace firefly::kernel::scheduler
