#include "firefly/scheduler/scheduler.hpp"

#include "firefly/intel64/int/interrupt.hpp"
#include "firefly/logger.hpp"
#include "firefly/scheduler/process.hpp"
#include "firefly/scheduler/thread.hpp"
#include "firefly/timer/timer.hpp"

namespace firefly::kernel::scheduler {
namespace {
frg::manual_box<Scheduler> schedulerSingleton;
Thread* idleThread;
Thread* currentThread;
}  // namespace

void KernelProcess() {
    debugLine << "KPROC\n"
              << fmt::endl;
    for (;;) {
        asm volatile("pause");
    }
}

Scheduler& Scheduler::accessor() {
    return *schedulerSingleton;
}

void scheduleHandler(uint8_t int_num, RegisterContext* regs) {
    Scheduler::accessor().schedule(regs);
}

void Scheduler::init() {
    schedulerSingleton.initialize();
    core::interrupt::registerInterruptHandler(scheduleHandler, core::interrupt::IPI_SCHEDULE);

    Process* idleProcess = Process::createIdleProcess("idle");
    idleThread = idleProcess->getMainThread();

    auto kproc = Process::createKernelProcess((void*)KernelProcess, "Kernel", nullptr);
    kproc->start();

    currentThread = nullptr;
    timer::startTimer();
    asm volatile("int $0xfd");
}

void Scheduler::registerProcess(Process* process) {
    procs.push_back(process);
}

void Scheduler::insertThread(Thread* thread) {
    threads.push_back(thread);
}

void Scheduler::yield() {
    asm volatile("int $0xfd");
}

void Scheduler::schedule(RegisterContext* regs) {
    if (threads.empty()) {
        currentThread = idleThread;
    } else {
        currentThread = threads[0];
    }
    doSwitch();
}

[[gnu::noreturn]] void Scheduler::doSwitch() {
    asm volatile(
        R"(mov %0, %%rsp;
        mov %%cr3, %%rax;
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
        iretq)" ::"r"(&currentThread->registers));
    __builtin_unreachable();
}

}  // namespace firefly::kernel::scheduler
