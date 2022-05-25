#include <stl/cstdlib/stdio.h>

#include "x86_64/trace/symbols.hpp"
#include "x86_64/logger.hpp"

namespace firefly::trace {

void trace_stack();
[[noreturn]] void panic(const char *msg) {
    kernel::info_logger << "\n**** Kernel panic ****\nReason: " << msg << "\n";
    trace_stack();

    while (1)
        ;
}

struct stackframe {
    struct stackframe *frame;
    uint64_t rip;
};

void trace_stack() {
    kernel::info_logger << "Stack trace:\n";
    struct stackframe *stkf;
    asm("mov %%rbp, %0"
        : "=r"(stkf));
    for (int i = 0; i < 10 && stkf; i++) {
        if (!backtrace(stkf->rip, i))
            break;
        stkf = stkf->frame;
    }
}
}  // namespace firefly::trace
