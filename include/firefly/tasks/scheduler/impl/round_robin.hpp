#pragma once

#include <frg/vector.hpp>

#include "firefly/memory-manager/allocator.hpp"
#include "firefly/tasks/scheduler/base.hpp"

namespace firefly::kernel::tasks {
class Scheduler : protected BaseScheduler<frg::vector<Task, Allocator>> {
public:
    Task* onSchedule(ContextRegisters* stack, uint8_t idx) {
        logLine << "schedule event on cpu#" << idx << "\n"
                << fmt::endl;

        if (unlikely(store.empty())) return nullptr;

        if (++n > store.size())
            n = 1;
        currentTask = &store[n - 1];
        return currentTask;
    }

    inline void registerTask(Task task) override {
        store.push(task);
    }

    inline void unregisterTask(Task task) override {
        logLine << "TODO: Implement frg::vector::erase()" << fmt::endl;
        // store.erase(task);
    }

private:
    uint64_t n;
};
}  // namespace firefly::kernel::tasks