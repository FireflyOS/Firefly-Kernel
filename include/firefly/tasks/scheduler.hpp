#pragma once

#include "firefly/logger.hpp"

#ifdef SCHEDULER_ALGORITHM_GOES_HERE
#else
#include "scheduler-impl/round_robin.hpp" /* Default to RR because that's all we have */
#endif

namespace firefly::kernel::tasks {
class Scheduler : SchedulerImpl {
public:
    static Scheduler& accessor();
    static void init();

    Task* getTask() const {
        return currentTask;
    }

    /**
     * Get next task
     */
    inline Task* schedule() {
        return onSchedule();
    }

    inline void addTask(Task task) {
        registerTask(task);
    }
};

}  // namespace firefly::kernel::tasks
