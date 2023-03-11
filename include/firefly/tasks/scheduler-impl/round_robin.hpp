#include <frg/vector.hpp>

#include "firefly/memory-manager/allocator.hpp"
#include "firefly/tasks/task.hpp"

namespace firefly::kernel::tasks {
class SchedulerImpl {
protected:
    Task* onSchedule() {
        if (++n > tasks.size())
            n = 1;
        currentTask = &tasks[n - 1];
        return currentTask;
    }

    inline void registerTask(Task task) {
        tasks.push(task);
    }

protected:
    frg::vector<Task, Allocator> tasks;
    Task* currentTask;

private:
    uint64_t n;
};
}  // namespace firefly::kernel::tasks