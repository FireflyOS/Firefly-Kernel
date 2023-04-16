#pragma once

#include <frg/vector.hpp>

#include "firefly/memory-manager/allocator.hpp"
#include "firefly/tasks/scheduler/base.hpp"

namespace firefly::kernel::tasks {
class Scheduler : public BaseScheduler<frg::vector<Task, Allocator>> {
public:
    Task* onSchedule() override {
        BaseScheduler::onSchedule();

        if (store.empty()) return nullptr;

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