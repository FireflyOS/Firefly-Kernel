#pragma once

#include <frg/vector.hpp>

#include "firefly/logger.hpp"
#include "firefly/memory-manager/allocator.hpp"
#include "firefly/tasks/task.hpp"

namespace firefly::kernel::tasks {
// TODO: Abstract, multiple sched impls
class Scheduler {
public:
    static Scheduler& accessor();
    static void init();

    Task* getTask() const {
        return currentTask;
    }

    /**
     * Get next task
     */
    Task* schedule() {
        if (++n > tasks.size())
            n = 1;
        currentTask = &tasks[n - 1];
        return currentTask;
    }

    void addTask(Task _task) {
        tasks.push(_task);
    }

protected:
    frg::vector<Task, Allocator> tasks;
    Task* currentTask;

private:
    uint64_t n;
};

}  // namespace firefly::kernel::tasks
