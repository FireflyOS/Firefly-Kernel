#pragma once

#include "firefly/logger.hpp"
#include "firefly/memory-manager/primary/primary_phys.hpp"
#include "firefly/tasks/context.hpp"

namespace firefly::kernel::tasks {
class Task {
public:
    Task(uintptr_t entrypoint, uintptr_t sp)
        : context(entrypoint, sp) {
    }

    void load(Registers* regs) {
        this->context.load(regs);
    }

    void save(const Registers* regs) {
        this->context.save(regs);
    }

    uintptr_t getRip() {
        return this->context.regs.rip;
    }

private:
    Context context;
};
}  // namespace firefly::kernel::tasks
