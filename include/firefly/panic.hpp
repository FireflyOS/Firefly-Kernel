#pragma once

#include "firefly/logger.hpp"
#include "firefly/trace/strace.hpp"

namespace firefly {

[[gnu::used]]
[[noreturn]] static void panic(const char *msg) {
    kernel::info_logger << "\n**** Kernel panic ****\nReason: " << msg << "\n";
    // trace::trace_callstack();

    while (1)
        asm volatile("hlt");
}

[[gnu::used]]
[[noreturn]] static void
assertion_failure_panic(const char *msg) {
    kernel::info_logger << "\n**** Kernel panic ****\nAssertion failed: `" << msg << "`\n";
    trace::trace_callstack();

    while (1)
        asm volatile("hlt");
}

}  // namespace firefly