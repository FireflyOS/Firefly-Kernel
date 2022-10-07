#pragma once

#include <stdarg.h>

#include <cstddef>
#include <cstring>
#include <type_traits>

#include "firefly/console/console.hpp"
#include "firefly/drivers/serial.hpp"
#include "frg/manual_box.hpp"
#include "libk++/fmt.hpp"

namespace firefly::kernel {
namespace logger {
static constexpr char endl = '\n';
}

class logger_interface {
protected:
    char buffer[512];

    virtual int logger_printf(const char *fmt, ...) {
        return 0;
    }

public:
    virtual ~logger_interface(){};

    void operator delete([[maybe_unused]] void *ptr) {
    }

    template <typename... VarArgs>
    logger_interface &operator<<(VarArgs... args) {
        ((*this << args), ...);
        return const_cast<logger_interface &>(*this);
    }

    logger_interface &operator<<(const char *cstring) {
        logger_printf("%s", cstring);
        return const_cast<logger_interface &>(*this);
    }

    logger_interface &operator<<(char *cstring) {
        logger_printf("%s", cstring);
        return const_cast<logger_interface &>(*this);
    }

    logger_interface &operator<<(char chr) {
        logger_printf("%c", chr);
        return const_cast<logger_interface &>(*this);
    }

    // Integral data types
    template <typename T>
    logger_interface &operator<<(T in) requires std::is_integral_v<T> {
        logger_printf("%d", in);
        return const_cast<logger_interface &>(*this);
    }

    const char *format(const char *fmt, ...) {
        // va_list ap;
        // va_start(ap, fmt);
        // size_t outLen = libkern::fmt::vsnprintf(buffer, sizeof(buffer), fmt, ap);
        // va_end(ap);
        // if (outLen >= sizeof(buffer))
        //     return nullptr;

        return const_cast<char *>("");
    }

    template <typename T = int8_t>
    const char *hex(T in, bool hexPrefix = true) {
        logger_printf(hexPrefix ? "0x%x" : "%x", in);
        return "";
    }

    constexpr char newline() const {
        return logger::endl;
    }

    constexpr char tab() const {
        return '\t';
    }
};

class ConsoleLogger : public logger_interface {
private:
    friend class frg::manual_box<ConsoleLogger>;

public:
    ConsoleLogger() {
    }

    static ConsoleLogger &log();
    static void init();

    int logger_printf(const char *fmt, ...);
};


class SerialLogger : public logger_interface {
private:
    friend class frg::manual_box<SerialLogger>;

public:
    SerialLogger() {
    }
    static SerialLogger &log();
    static void init();

    int logger_printf(const char *fmt, ...);
};

}  // namespace firefly::kernel

#include "libk++/fmt.hpp"

namespace firefly {

namespace fmt_options {
struct __endl {};

struct __hex {
    int base;
};

struct __dec {
    int base;
};

struct __bin {
    int base;
};

struct __oct {
    int base;
};
}  // namespace fmt_options

namespace fmt {
constexpr fmt_options::__endl endl;
constexpr fmt_options::__bin bin{ 2 };
constexpr fmt_options::__oct oct{ 8 };
constexpr fmt_options::__dec dec{ 10 };
constexpr fmt_options::__hex hex{ 16 };
}  // namespace fmt

namespace __writer {
struct fb {
    void operator()(const char *s) {
        kernel::console::write(s);
    }
};

struct serial {
    void operator()(const char *s) {
        using namespace kernel;
        io::SerialPort port = io::SerialPort(io::SerialPort::COM1, io::SerialPort::BAUD_BASE);
        port.send_chars(s, -1);
    }
};
};  // namespace __writer

template <typename F>
class ostream : format {
private:
    template <typename T>
    void vprintf(T in) {
        do_format(in);
    }

public:
	constexpr ostream() = default;

    template <typename T>
    ostream &operator<<(T arg) {
        vprintf<T>(arg);
        return *this;
    }

    ostream &operator<<(fmt_options::__endl) {
        flush();
        return *this;
    }

    // Special case:
    // Handles the base of a number using fmt_options::__{dec, hex, etc}
    template <typename NumberBase>
    ostream &operator<<(NumberBase nb) requires std::is_class_v<NumberBase> {
        base = static_cast<format_base>(nb.base);
        return *this;
    }

    inline void flush() {
        writeCallback(buffer);
        writer_position = 0;
    }

private:
    F writeCallback;
};

class log : public ostream<__writer::fb> {
    constexpr log() = default;
};
class dbg : public ostream<__writer::serial> {
    constexpr dbg() {
    }
};

extern constinit log logLine;
extern constinit dbg debugLine;

// extern constinit frg::manual_box<log> logLine;
// extern constinit frg::manual_box<dbg> debugLine;

}  // namespace firefly