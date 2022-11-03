#pragma once

namespace firefly::kernel::scheduler {
class Process;
}

#include "firefly/scheduler/thread.hpp"

namespace firefly::kernel::scheduler {
enum ProcessState : int {
    Running = 0,
    Stopping,
    Stopped
};

class Process {
    friend struct Thread;

public:
    static Process* createIdleProcess(const char* name);
    static Process* createKernelProcess(void* entry, const char* name, Process* parent);

    Process(int pid, const char* _name, Process* _parent);

    void start();

    inline Process* parentProcess() const {
        return this->parent;
    }

    inline Thread* getMainThread() {
        return mainThread;
    }

private:
    bool started;
    bool isIdleProcess = false;
    int state = ProcessState::Running;

    Process* parent = nullptr;
    Thread* mainThread;
};
}  // namespace firefly::kernel::scheduler
