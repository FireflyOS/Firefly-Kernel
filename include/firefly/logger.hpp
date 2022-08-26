#pragma once

#include <stdarg.h>

#include "firefly/console/stivale2-term.hpp"
#include "libk++/fmt.hpp"

namespace firefly::kernel {

namespace logger {
static constexpr char endl = '\n';
}


class logger_impl {
private:
    char buffer[512];

public:
    template <typename... VarArgs>
    logger_impl &operator<<(VarArgs... args) const {
        ((*this << args), ...);
        return const_cast<logger_impl &>(*this);
    }

    logger_impl &operator<<(const char *cstring) const {
        libkern::fmt::printf("%s", cstring);
        // printf("%s", cstring);
        return const_cast<logger_impl &>(*this);
    }

    logger_impl &operator<<(char *cstring) const {
        libkern::fmt::printf("%s", cstring);
        return const_cast<logger_impl &>(*this);
    }

    logger_impl &operator<<(char chr) const {
        libkern::fmt::printf("%c", chr);
        return const_cast<logger_impl &>(*this);
    }

    // Integral data types
    template <typename T>
    logger_impl &operator<<(T in) const {
        libkern::fmt::printf("%d", in);
        return const_cast<logger_impl &>(*this);
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

    template <typename T>
    const char *hex(T in) const {
        libkern::fmt::printf("0x%x", in);
        return "";
    }

    constexpr char newline() const {
        return logger::endl;
    }

    constexpr char tab() const {
        return '\t';
    }
};

static logger_impl info_logger;
}  // namespace firefly::kernel