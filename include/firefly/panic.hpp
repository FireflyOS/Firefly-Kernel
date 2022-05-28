#pragma once

#include "firefly/logger.hpp"
#include "firefly/trace/strace.hpp"

namespace firefly {

[[noreturn]] static void panic(const char *msg) {
    kernel::info_logger << "\n**** Kernel panic ****\nReason: " << msg << "\n";
    trace::trace_callstack();
    
    while (1)
        asm volatile("hlt");
}

}  // namespace firefly