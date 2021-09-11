#include <stl/cstdlib/stdio.h>
#include <x86_64/libk++/iostream.h>
#include <stl/array.h>

#include <x86_64/trace/symbols.hpp>

namespace icelyos::trace {

void trace_stack();
[[noreturn]] void panic(const char *P_REASON) {
    printf("**** Kernel panic ****\nReason: %s\n\n", P_REASON);
    trace_stack();

    while (1)
        ;
}

struct stackframe {
    struct stackframe *frame;
    uint64_t rip;
};

void trace_stack() {
    printf("Stack trace:\n");
    struct stackframe *stkf;
    asm("mov %%rbp, %0"
        : "=r"(stkf));
    for (int i = 0; i < 10 && stkf; i++) {
        if (!backtrace(stkf->rip, i))
            break;
        stkf = stkf->frame;
    }
}
}  // namespace icelyos::trace
