#include "firefly/scheduler/scheduler.hpp"

#include <cstdint>
#include <cstring>

#include "firefly/intel64/int/interrupt.hpp"
#include "firefly/intel64/uspace.hpp"
#include "firefly/limine.hpp"
#include "firefly/memory-manager/allocator.hpp"
#include "firefly/memory-manager/mm.hpp"
#include "firefly/memory-manager/virtual/vspace.hpp"
#include "frg/manual_box.hpp"
#include "frg/vector.hpp"
#include "libk++/memory.hpp"

static volatile struct limine_module_request module_request = {
    .id = LIMINE_MODULE_REQUEST, .revision = 0, .response = nullptr
};

namespace firefly::kernel::scheduler {
namespace {
frg::manual_box<Scheduler> schedulerSingleton;
}

void Scheduler::reschedule(core::interrupt::iframe regs) {
    if (tasks.empty()) return;

    if (++currTask > tasks.size())
        currTask = 0;

    tasks[currTask].regs = regs;

    auto newTask = tasks[currTask];
    newTask.switchPageMap();
}

Scheduler& Scheduler::accessor() {
    return *schedulerSingleton;
}

void Scheduler::registerTask(Task task) {
    tasks.push(task);
}

void Scheduler::init() {
    schedulerSingleton.initialize();
    auto sched = schedulerSingleton.get();
    for (;;) {
        asm volatile("hlt");
    }
}
}  // namespace firefly::kernel::scheduler
