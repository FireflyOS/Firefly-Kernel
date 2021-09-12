#pragma once

namespace firefly::trace {
constexpr const char* PM_OUTOFMEMORY     = "OUT_OF_MEMORY_EXCEPTION";
constexpr const char* PM_DRIVERERROR_PK  = "PS2_KEYBOARD_DRIVER_ERROR";
constexpr const char* PM_DRIVERERROR_S   = "SERIAL_PORT_DRIVER_ERROR";
constexpr const char* PM_DRIVERERROR_V   = "VBE_DRIVER_ERROR";
constexpr const char* PM_MANUALLYCRASHED = "MANUALLY_CRASHED";

constexpr const int   PC_OUTOFMEMORY     = 0x0E000F;
constexpr const int   PC_DRIVERERROR_PK  = 0x0F3C40;
constexpr const int   PC_DRIVERERROR_S   = 0x0F1450;
constexpr const int   PC_DRIVERERROR_V   = 0x0FFF00;
constexpr const int   PC_MANUALLYCRASHED = 0xFFFFFF;

//PM — Panic Message
//PC — Panic Code
void trace_stack();
void panic(const char *P_REASON, const int P_ERRCODE);
}  // namespace firefly::trace