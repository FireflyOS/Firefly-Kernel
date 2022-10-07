#include "firefly/logger.hpp"

#include <cstddef>

#include "frg/manual_box.hpp"

namespace firefly {
constinit log logLine = {};
constinit dbg debugLine = {};

// constinit frg::manual_box<log> logLine;
// constinit frg::manual_box<log> debugLine;
}  // namespace firefly

namespace firefly::kernel {

frg::manual_box<ConsoleLogger> consoleLoggerSingleton;
frg::manual_box<SerialLogger> serialLoggerSingleton;

ConsoleLogger &ConsoleLogger::log() {
    return *consoleLoggerSingleton;
}

SerialLogger &SerialLogger::log() {
    return *serialLoggerSingleton;
}

void ConsoleLogger::init() {
    consoleLoggerSingleton.initialize();
}

void SerialLogger::init() {
    serialLoggerSingleton.initialize();
}

int ConsoleLogger::logger_printf(const char *fmt, ...) {
    // va_list ap;
    // va_start(ap, fmt);
    // size_t outLen = libkern::fmt::vsnprintf(buffer, sizeof(buffer), fmt, ap);
    // va_end(ap);

    // if (outLen >= sizeof(buffer))
    //     return -1;

    // firefly::kernel::console::write(buffer);
    return 0;
}


int SerialLogger::logger_printf(const char *fmt, ...) {
    // io::SerialPort port = io::SerialPort(io::SerialPort::COM1, io::SerialPort::BAUD_BASE);
    // va_list ap;
    // va_start(ap, fmt);
    // size_t outLen = libkern::fmt::vsnprintf(buffer, sizeof(buffer), fmt, ap);
    // va_end(ap);

    // if (outLen >= sizeof(buffer))
    //     return -1;

    // port.send_chars(buffer, -1);
    return 0;
}

}  // namespace firefly::kernel
