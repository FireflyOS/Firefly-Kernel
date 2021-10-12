#include <stl/cstdlib/stdio.h>
#include <x86_64/libk++/iostream.h>
#include <stl/array.h>

#include <x86_64/trace/symbols.hpp>

namespace firefly::trace {

void trace_stack();

/*
    Kernel panic
*/
[[noreturn]] void panic(const char *P_REASON, const int P_ERRCODE) {
    printf("**** Kernel panic ****\nReason    : %s\nError Code: 0x%X\n\n", P_REASON, P_ERRCODE);
    trace_stack();

    while (1)
        ;
}

/*
    Stack frame
*/
struct stackframe {
    struct stackframe *frame;
    uint64_t rip;
};

/*
    Prints contents of stack
*/
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
}  // namespace firefly::trace
