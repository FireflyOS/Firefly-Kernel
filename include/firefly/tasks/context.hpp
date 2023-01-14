#pragma once

#include "firefly/intel64/cpu/cpu.hpp"
#include "libk++/memory.hpp"

namespace firefly::kernel::tasks {

/**
 * Class containing everything related to task context
 * this includes registers, stack, cr0 and so on
 */
class Context {
    friend class Task;

public:
    Context(uintptr_t ip, uintptr_t sp) {
        memset(&regs, 0, sizeof(Registers));
        // set entrypoint
        regs.rip = ip;
        // TODO: change this
        regs.rflags = (1 << 9) | (1 << 1);
        // TODO: user mode data selectors
        regs.cs = core::gdt::gdtEntryOffset(SegmentSelector::kernCS);
        regs.ss = core::gdt::gdtEntryOffset(SegmentSelector::kernDS);
        // setup the stack registers
        // TODO: change this probably
        regs.rbp = (int64_t)sp;
        regs.rsp = (int64_t)sp + 4096 * 2;
    }

    void save(const Registers* regs);
    void load(Registers* regs);

private:
    Registers regs;
};

}  // namespace firefly::kernel::tasks
