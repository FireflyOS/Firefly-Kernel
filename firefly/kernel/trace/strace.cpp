#include <stl/cstdlib/stdio.h>

#include "firefly/logger.hpp"
#include "firefly/trace/symbols.hpp"

namespace firefly::trace {

// [[noreturn]] void panic(const char *msg) {
//     kernel::info_logger << "\n**** Kernel panic ****\nReason: " << msg << "\n";
//     trace_stack();

//     while (1)
//         ;
// }

struct stackframe {
    struct stackframe *frame;
    uint64_t rip;
};

void trace_callstack() {
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
