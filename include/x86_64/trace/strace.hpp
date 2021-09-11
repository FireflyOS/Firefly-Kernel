#pragma once

namespace icelyos::trace {
constexpr const char* P_OUTOFMEMORY     = "Out of Memory Error";
constexpr const char* P_DRIVERERROR     = "Driver Error";
constexpr const char* P_MANUALLYCRASHED = "Manually Crashed";
void trace_stack();
void panic(const char *msg);
}  // namespace icelyos::trace