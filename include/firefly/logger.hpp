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
        va_list ap;
        va_start(ap, fmt);
        size_t outLen = libkern::fmt::vsnprintf(buffer, sizeof(buffer), fmt, ap);
        va_end(ap);
        if (outLen >= sizeof(buffer))
            return nullptr;

        return const_cast<char *>(buffer);
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
