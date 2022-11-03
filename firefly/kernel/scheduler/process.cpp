#include "firefly/scheduler/process.hpp"

#include <cstdint>

#include "firefly/logger.hpp"
#include "firefly/scheduler/scheduler.hpp"
#include "firefly/scheduler/thread.hpp"

namespace firefly::kernel::scheduler {
void IdleProcess() {
    debugLine << "IDLEPROC\n"
              << fmt::endl;
    for (;;) {
        asm volatile("pause");
    }
}

Process* Process::createIdleProcess(const char* name) {
    Process* proc = new Process(0, name, nullptr);
    proc->mainThread->registers.rip = reinterpret_cast<uintptr_t>(IdleProcess);
    proc->mainThread->registers.rsp = reinterpret_cast<uintptr_t>(proc->mainThread->kernelStack);
    proc->mainThread->registers.rbp = reinterpret_cast<uintptr_t>(proc->mainThread->kernelStack);

    proc->isIdleProcess = true;

    Scheduler::accessor().registerProcess(proc);
    return proc;
}

Process* Process::createKernelProcess(void* entry, const char* name, Process* parent) {
    Process* proc = new Process(0, name, nullptr);

    proc->mainThread->registers.rip = reinterpret_cast<uintptr_t>(IdleProcess);
    proc->mainThread->registers.rsp = reinterpret_cast<uintptr_t>(proc->mainThread->kernelStack);
    proc->mainThread->registers.rbp = reinterpret_cast<uintptr_t>(proc->mainThread->kernelStack);

    Scheduler::accessor().registerProcess(proc);
    return proc;
}

void Process::start() {
}

Process::Process(int pid, const char* _name, Process* _parent)
    : parent(_parent) {
    mainThread = new Thread(this, 1);
}
}  // namespace firefly::kernel::scheduler
