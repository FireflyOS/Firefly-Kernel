#include <i386/libk++/iostream.h>

#include <i386/trace/symbols.hpp>


namespace firefly::trace {

void trace_stack();
void panic(const char *msg) {
    printf("**** Kernel panic ****\n%s", msg);
    trace_stack();

    while (1)
        ;
}

struct stackframe {
    struct stackframe *frame;
    uint32_t eip;
};

void trace_stack() {
    struct stackframe *stkf;
    asm("mov %%ebp, %0"
        : "=r"(stkf));
    for (int i = 0; i < 10 && stkf; i++) {
        if (!backtrace(stkf->eip))
            break;
        stkf = stkf->frame;
    }
}
}  // namespace firefly::trace