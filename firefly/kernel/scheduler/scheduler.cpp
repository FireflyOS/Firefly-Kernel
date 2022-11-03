#include "firefly/scheduler/scheduler.hpp"

#include <cstdint>
#include <cstring>

#include "firefly/intel64/int/interrupt.hpp"
#include "firefly/intel64/uspace.hpp"
#include "firefly/limine.hpp"
#include "firefly/memory-manager/allocator.hpp"
#include "firefly/memory-manager/mm.hpp"
#include "firefly/memory-manager/primary/primary_phys.hpp"
#include "firefly/memory-manager/stack.hpp"
#include "firefly/memory-manager/virtual/vspace.hpp"
#include "firefly/timer/timer.hpp"
#include "frg/manual_box.hpp"
#include "frg/vector.hpp"
#include "libk++/memory.hpp"

namespace firefly::kernel::scheduler {
namespace {
frg::manual_box<Scheduler> schedulerSingleton;
}

void Scheduler::reschedule(RegisterContext* regs) {
    if (tasks.empty()) return;

    auto oldTask = &tasks[currTask];
    oldTask->regs = *regs;

    if (++currTask > tasks.size())
        currTask = 0;

    auto newTask = tasks[currTask];
    asm volatile(
        R"(mov %0, %%rsp;
	mov %1, %%rax;
	pop %%r15;
        pop %%r14;
        pop %%r13;
        pop %%r12;
        pop %%r11;
        pop %%r10;
        pop %%r9;
        pop %%r8;
        pop %%rbp;
        pop %%rdi;
        pop %%rsi;
        pop %%rdx;
        pop %%rcx;
        pop %%rbx;
	
	mov %%rax, %%cr3
	pop %%rax
	addq $8, %%rsp
	iretq)" ::"r"(&newTask.regs),
        "r"(&newTask.cr3));
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
    tt.regs.rsp = kStack->get().rsp;
    tt.regs.rbp = kStack->get().rbp;
    sched->registerTask(tt);

    auto tt2 = Task("test2");
    tt2.regs.rip = reinterpret_cast<int64_t>(__task2_entry);
    tt2.regs.rsp = kStack->get().rsp;
    tt2.regs.rbp = kStack->get().rbp;
    sched->registerTask(tt2);

    timer::startTimer();

    for (;;) {
    }
}
}  // namespace firefly::kernel::scheduler
