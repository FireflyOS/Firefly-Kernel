#pragma once

namespace firefly::trace {
void trace_stack();
void panic(const char *msg);
}  // namespace firefly::trace