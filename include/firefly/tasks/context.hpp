#pragma once

#include "firefly/intel64/cpu/registers.hpp"
#include "firefly/intel64/gdt/gdt.hpp"
#include "firefly/intel64/gdt/tss.hpp"
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
        memset(&regs, 0, sizeof(ContextRegisters));
        // set entrypoint
        regs.rip = ip;
        // set rflags
        RFlags rflags;
        rflags.fields.interruptEnable = 1;
        rflags.fields.carry = 1;
        regs.rflags = rflags.All;
        // TODO: user mode data selectors
        regs.cs = core::gdt::gdtEntryOffset(SegmentSelector::kernCS);
        regs.ss = core::gdt::gdtEntryOffset(SegmentSelector::kernDS);
        // setup the stack registers
        // TODO: change this
        regs.rbp = (int64_t)sp;
        regs.rsp = (int64_t)sp + 4096 * 2;
    }

    inline void save(const ContextRegisters* regs) {
        this->regs = *regs;
    }
    inline void load(ContextRegisters* regs) {
        *regs = this->regs;
    }

private:
    ContextRegisters regs;
};

}  // namespace firefly::kernel::tasks
