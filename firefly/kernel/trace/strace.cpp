#include "firefly/logger.hpp"
#include "firefly/trace/symbols.hpp"

namespace firefly::trace {

struct stackframe {
    struct stackframe *frame;
    uint64_t rip;
};

void trace_callstack() {
    kernel::ConsoleLogger::log() << "Stack trace:\n";

    stackframe *stkf;
    asm("mov %%rbp, %0"
        : "=r"(stkf));

    for (int i = 0; i < 10; i++) {
        if (!backtrace(stkf->rip, i))
            return;

        stkf = stkf->frame;
    }
}
}  // namespace firefly::trace
