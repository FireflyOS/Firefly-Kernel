#pragma once
#include "firefly/tasks/task.hpp"

namespace firefly::kernel::tasks {
template <typename T>
class BaseScheduler {
protected:
    BaseScheduler() = default;
    virtual ~BaseScheduler() = default;

    virtual Task* onSchedule() {
        return nullptr;
    }
    virtual inline void registerTask(Task task) = 0;
    virtual inline void unregisterTask(Task task) = 0;

protected:
    T store;
    Task* currentTask;
};
}  // namespace firefly::kernel::tasks