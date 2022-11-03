#pragma once

#include <cstring>

#include "firefly/intel64/cpu/cpu.hpp"
#include "firefly/intel64/gdt/gdt.hpp"
#include "firefly/memory-manager/primary/primary_phys.hpp"

namespace firefly::kernel::scheduler {
struct Thread;
}

#include "firefly/scheduler/process.hpp"

namespace firefly::kernel::scheduler {
struct Thread {
    Process* parent;            // Parent process
    RegisterContext registers;  // Registers

    void* stack = nullptr;  // pointer to initial stack
    void* kernelStackBase;
    void* kernelStack;

    int tid;

    Thread(class Process* _parent, int _tid)
        : parent(_parent), tid(_tid) {
        memset(&registers, 0, sizeof(RegisterContext));
        registers.rflags = 0x202;  // IF - interrupt flag, bit 1should be 1
        registers.cs = core::gdt::gdtEntryOffset(SegmentSelector::kernCS);
        registers.ss = core::gdt::gdtEntryOffset(SegmentSelector::kernDS);

        kernelStackBase = mm::Physical::must_allocate(128 * 4096);
        kernelStack = reinterpret_cast<uint8_t*>(kernelStackBase) + 128 * 4096;
    }
};
}  // namespace firefly::kernel::scheduler
