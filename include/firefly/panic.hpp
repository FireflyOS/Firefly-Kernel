#pragma once

#include "firefly/logger.hpp"
#include "firefly/trace/strace.hpp"

namespace firefly {

[[gnu::used]] [[noreturn]] static void
panic(const char *msg) {
    logLine << "\n**** Kernel panic ****\nReason: " << msg << "\n"
            << fmt::endl;
    trace::trace_callstack();

    while (1)
        // asm volatile("cli; hlt");
        asm volatile("sti; hlt");
}

[[gnu::used]] [[noreturn]] static void
assertion_failure_panic(const char *msg) {
    logLine << "\n**** Kernel panic ****\nAssertion failed: `" << msg << "\n"
            << fmt::endl;
    trace::trace_callstack();

    while (1)
        asm volatile("cli; hlt");
}

}  // namespace firefly
