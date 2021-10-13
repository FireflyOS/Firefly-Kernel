#pragma once

#include <stl/cstdlib/cstdint.h>

namespace firefly::kernel::io::legacy {
    bool serial_port_init();
    int  isGotSignal();
    char readSerial();
    int  isTransmitEmpty();
    void writeCharSerial(char a);
    void writeSerial(const char* data, size_t size, bool istoupper);
    int writeTextSerial(const char* fmt, ...);
}