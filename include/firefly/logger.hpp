#pragma once

#include <stdarg.h>
// #include <stl/cstdlib/cstring.h>
#include <stl/cstdlib/stdio.h>

#include "firefly/fb/stivale2-term.hpp"

extern int vsnprintf(char *str, size_t size, const char *fmt, va_list ap);

namespace firefly::kernel {

namespace logger {
static constexpr char endl = '\n';
}


class logger_impl {
public:
    template <typename... VarArgs>
    logger_impl &operator<<(VarArgs... args) const {
        ((*this << args), ...);
        return const_cast<logger_impl &>(*this);
    }

    logger_impl &operator<<(const char *cstring) const {
        printf("%s", cstring);
        return const_cast<logger_impl &>(*this);
    }
    
    logger_impl &operator<<(char *cstring) const {
        printf("%s", cstring);
        return const_cast<logger_impl &>(*this);
    }

    logger_impl &operator<<(char chr) const {
        printf("%c", chr);
        return const_cast<logger_impl &>(*this);
    }

    // Integral data types
    template <typename T>
    logger_impl &operator<<(T in) const {
        printf("%d", in);
        return const_cast<logger_impl &>(*this);
    }

    const char *fmt(const char *f, ...) {
        static char buffer[512];
        va_list ap;
        va_start(ap, f);
        vsnprintf((char *)&buffer, (size_t)511, f, ap);
        va_end(ap);

        return const_cast<char *>(buffer);
    }

    template <typename T>
    const char *hex(T in) const {
        printf("0x%x", in);
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