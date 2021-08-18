#include <stl/cstdlib/stdio.h>
#include <x86_64/libk++/iostream.h>

#include <x86_64/trace/symbols.hpp>

namespace firefly::trace {

void trace_stack();
void panic(const char *msg) {
    printf("**** Kernel panic ****\n%s", msg);
    // trace_stack();

    while (1)
        ;
}

struct stackframe {
    struct stackframe *frame;
    uint64_t rip;
};

void trace_stack() {
    struct stackframe *stkf;
    asm("mov %%rbp, %0"
        : "=r"(stkf));
    for (int i = 0; i < 10 && stkf; i++) {
        // if (!backtrace(stkf->rip))
        //     break;
        stkf = stkf->frame;
    }
}
}  // namespace firefly::trace
