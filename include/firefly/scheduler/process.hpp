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

    inline const char* getName() const {
        return name;
    }

    inline int getPid() const {
        return pid;
    }

private:
    bool started;
    bool isIdleProcess = false;

    int state = ProcessState::Running;
    int pid = -1;

    const char* name;

    Process* parent = nullptr;
    Thread* mainThread;
};
}  // namespace firefly::kernel::scheduler
