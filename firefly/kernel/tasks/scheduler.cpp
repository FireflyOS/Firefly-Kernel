#include "firefly/tasks/scheduler.hpp"

namespace firefly::kernel::tasks {

frg::manual_box<Scheduler> schedSingleton;
Scheduler& Scheduler::accessor() {
    return *schedSingleton;
}
void Scheduler::init() {
    schedSingleton.initialize();
}
}  // namespace firefly::kernel::tasks
