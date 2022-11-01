#include "firefly/scheduler/scheduler.hpp"

#include <cstdint>
#include <cstring>

#include "firefly/intel64/int/interrupt.hpp"
#include "firefly/intel64/uspace.hpp"
#include "firefly/limine.hpp"
#include "firefly/memory-manager/allocator.hpp"
#include "firefly/memory-manager/mm.hpp"
#include "firefly/memory-manager/primary/primary_phys.hpp"
#include "firefly/memory-manager/virtual/vspace.hpp"
#include "firefly/timer/timer.hpp"
#include "frg/manual_box.hpp"
#include "frg/vector.hpp"
#include "libk++/memory.hpp"

namespace firefly::kernel::scheduler {
namespace {
frg::manual_box<Scheduler> schedulerSingleton;
}

void Scheduler::preempt() {
    asm volatile("mov %%rsp, %0" ::"r"(tasks[currTask].regs.rsp));
    asm volatile("mov %%rbx, %0" ::"r"(tasks[currTask].regs.gpr.rbx));
    asm volatile("mov %%rbp, %0" ::"r"(tasks[currTask].regs.gpr.rbp));
    asm volatile("mov %%r12, %0" ::"r"(tasks[currTask].regs.gpr.r12));
    asm volatile("mov %%r13, %0" ::"r"(tasks[currTask].regs.gpr.r13));
    asm volatile("mov %%r14, %0" ::"r"(tasks[currTask].regs.gpr.r14));
    asm volatile("mov %%r15, %0" ::"r"(tasks[currTask].regs.gpr.r15));
    asm volatile("push %0" ::"r"(tasks[currTask].regs.rip));
}

void Scheduler::reschedule(core::interrupt::iframe regs) {
    if (tasks.empty()) return;

    tasks[currTask].regs = regs;

    // if (++currTask > tasks.size())
    // currTask = 0;

    if (currTask == 0)
        currTask = 1;
    else
        currTask = 0;

    auto newTask = tasks[currTask];
    preempt();
    // newTask.switchPageMap();
}

Scheduler& Scheduler::accessor() {
    return *schedulerSingleton;
}

void Scheduler::registerTask(Task task) {
    tasks.push(task);
}

static void __task_entry() {
    uint64_t runTime = 0;
    debugLine << "Task 1\n"
              << fmt::endl;
    for (;;) {
        runTime++;
    }
}

static void __task2_entry() {
    uint64_t runTime = 0;
    debugLine << "Task 2\n"
              << fmt::endl;
    for (;;) {
        runTime++;
    }
}

void Scheduler::init() {
    schedulerSingleton.initialize();
    auto sched = schedulerSingleton.get();

    auto tt = Task("test");
    tt.regs.rip = reinterpret_cast<int64_t>(__task_entry);

    auto tt2 = Task("test2");
    tt2.regs.rip = reinterpret_cast<int64_t>(__task2_entry);

    sched->registerTask(tt2);
    sched->registerTask(tt);

    timer::startTimer();

    for (;;) {
    }
}
}  // namespace firefly::kernel::scheduler
