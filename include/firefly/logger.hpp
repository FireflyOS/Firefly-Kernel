#pragma once

#include <stdarg.h>

#include <cstddef>

#include "firefly/console/console.hpp"
#include "firefly/drivers/serial.hpp"
#include "libk++/fmt.hpp"

namespace firefly {

namespace fmt_options {
struct __endl {};
struct __base {
    int base;
};

struct __hex : __base {};
struct __dec : __base {};
struct __bin : __base {};
struct __oct : __base {};
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
        empty_buffer();
        writer_position = 0;
    }


private:
    inline void empty_buffer() {
        for (size_t i = 0; i < writer_position; i++)
            buffer[i] = 0;
    }

private:
    F writeCallback;
};

class log : public ostream<__writer::fb> {};
class dbg : public ostream<__writer::serial> {};

extern log logLine;
extern dbg debugLine;

}  // namespace firefly