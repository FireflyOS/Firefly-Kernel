#include "firefly/logger.hpp"
#include "firefly/trace/symbols.hpp"

namespace firefly::trace {

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
