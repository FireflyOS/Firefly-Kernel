#pragma once

#include <stdarg.h>

#include <cstddef>

namespace firefly::libkern::fmt {
int vsnprintf(char* str, size_t size, const char* fmt, va_list ap);
int printf(const char* fmt, ...);
void itoa(size_t num, char* str, int base);
int atoi(const char* str);
char* strrev(char* str);
}  // namespace firefly::libkern::fmt


#include <type_traits>

namespace firefly {
struct format {
private:
    static constexpr const int BUFF_LEN{ 512 };

private:
    char itoc(int num);
    char itoh(int num, bool upper);
    void itoa(size_t num, char* s, int base);
    char* reverse(char* s);
    size_t len(const char* s);

public:
    format()
        : base{ format_base::Dec } {
    }

    template <typename T>
    void do_format(T in) requires std::is_integral_v<T> {
        char out[32];
        itoa(in, out, static_cast<int>(base));
        appendToBuffer(out);
    }

    void do_format(char in);
    void do_format(char* in);
    void do_format(const char* in);

    void appendToBuffer(char c);
    void appendToBuffer(const char* s);

protected:
    enum format_base : int {
        Bin = 2,
        Oct = 8,
        Dec = 10,
        Hex = 16
    };

    char buffer[BUFF_LEN]{ 0 };
    size_t writer_position{ 0 };
    format_base base;
};

}  // namespace firefly