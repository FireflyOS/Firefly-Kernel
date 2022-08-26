#include "libk++/fmt.hpp"

#include "firefly/console/stivale2-term.hpp"

namespace firefly::libkern::fmt {

size_t strlen(const char* str) {
    size_t n = 0;
    while (*str++)
        n++;
    return n;
}

size_t strnlen(const char* str) {
    return strlen(str) + 1;
}


char itoc(int num) {
    return '0' + num;
}

char itoh(int num, bool upper) {
    if (upper)
        return num - 10 + 'A';
    return num - 10 + 'a';
}

char* strrev(char* src) {
    static char temp;
    int src_string_index = 0;
    int last_char = strlen(src) - 1;

    for (; src_string_index < last_char; src_string_index++) {
        temp = src[src_string_index];  // Save current character
        src[src_string_index] =
            src[last_char];     // Swap out the current char with the last char
        src[last_char] = temp;  // Swap out last character with the current character
        last_char--;
    }

    src[strlen(src) - 1 + 1] = '\0';

    return src;
}

void itoa(size_t num, char* str, int base) {
    size_t buffer_sz = 20;
    size_t counter = 0;
    size_t digit = 0;

    while (num != 0 && counter < buffer_sz) {
        digit = (num % base);
        if (digit > 9) {
            str[counter++] = itoh(digit, false);
        } else {
            str[counter++] = itoc(digit);
        }
        num /= base;
    }

    str[counter] = '\0';
    strrev(str);
}

void itoa(size_t num, char* str, int base, bool upper) {
    size_t buffer_sz = 20;
    size_t counter = 0;
    size_t digit = 0;

    if (!upper) {
        itoa(num, str, base);
        return;
    } else {
        while (num != 0 && counter < buffer_sz - 1) {
            digit = (num % base);
            if (digit > 9) {
                str[counter++] = itoh(digit, true);
            } else {
                str[counter++] = itoc(digit);
            }
            num /= base;
        }
    }

    str[counter] = '\0';
    strrev(str);
}

int atoi(const char* str) {
    int ret = 0;
    int i;

    for (i = 0; str[i]; i++) {
        ret = ret * 10 + str[i] - '0';
    }

    return ret;
}

char buffer[512];
int printf(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buffer, (size_t)512, fmt, ap);
    va_end(ap);

    firefly::kernel::device::stivale2_term::write(buffer);

    return 0;
}

int vsnprintf(char* str, size_t size, const char* fmt, va_list ap) {
    while (*fmt != '\0') {
        switch (*fmt) {
            case '%': {
                switch (*++fmt) {
                    case 'c': {
                        auto arg = va_arg(ap, int);
                        *str++ = arg;
                        break;
                    }

                    case 's': {
                        auto arg = va_arg(ap, char*);
                        size_t len = strlen(arg);
                        for (size_t j = 0; j < len; j++)
                            *str++ = arg[j];

                        break;
                    }

                    case 'i':
                    case 'd': {
                        uint64_t arg = va_arg(ap, uint64_t);
                        if (arg == 0)
                            *str++ = '0';
                        else {
                            char res[20];
                            itoa(arg, res, 10);

                            size_t len = strlen(res);
                            for (size_t j = 0; j < len; j++)
                                *str++ = res[j];
                        }
                        break;
                    }

                    case 'x': {
                        uint64_t arg = va_arg(ap, uint64_t);
                        if (arg == 0)
                            *str++ = '0';
                        else {
                            char res[20];
                            itoa(arg, res, 16);

                            size_t len = strlen(res);
                            for (size_t j = 0; j < len; j++)
                                *str++ = res[j];
                        }
                        break;
                    }

                    case 'X': {
                        uint64_t arg = va_arg(ap, uint64_t);
                        char res[20];
                        if (arg == 0)
                            *str++ = '0';
                        else {
                            itoa(arg, res, 16, true);

                            size_t len = strlen(res);
                            for (size_t j = 0; j < len; j++)
                                *str++ = res[j];
                        }
                        break;
                    }
                }

                break;
            }

            default:
                *str++ = *fmt;
                break;
        }
        ++fmt;
    }
    *str = '\0';

    return 0;
}
}  // namespace firefly::libkern::fmt