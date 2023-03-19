#include "libk++/fmt.hpp"

#include <cstdint>

#include "cstdlib/cassert.h"

namespace firefly {

char format::itoc(int num) {
    return '0' + num;
}

char format::itoh(int num, bool upper) {
    if (upper)
        return num - 10 + 'A';
    return num - 10 + 'a';
}

char* format::reverse(char* s) {
    char temp;
    int src_string_index = 0;
    int last_char = len(s) - 1;

    for (; src_string_index < last_char; src_string_index++) {
        temp = s[src_string_index];  // Save current character
        s[src_string_index] =
            s[last_char];     // Swap out the current char with the last char
        s[last_char] = temp;  // Swap out last character with the current character
        last_char--;
    }

    s[len(s) - 1 + 1] = '\0';

    return s;
}

void format::itoa(size_t num, char* s, int base) {
    size_t buffer_sz = 20;
    size_t counter = 0;
    size_t digit = 0;

    if (num == 0) {
        s[0] = '0';
        s[1] = '\0';
        return;
    }

    while (num != 0 && counter < buffer_sz) {
        digit = (num % base);
        if (digit > 9)
            s[counter++] = itoh(digit, false);
        else
            s[counter++] = itoc(digit);

        num /= base;
    }

    s[counter] = '\0';
    reverse(s);
}

size_t format::len(const char* s) {
    size_t n = 0;

    while (*s++)
        n++;

    return n;
}

void format::do_format(char in) {
    appendToBuffer(in);
}

void format::do_format(char* in) {
    appendToBuffer(in);
}

void format::do_format(const char* in) {
    appendToBuffer(in);
}

void format::appendToBuffer(char c) {
    assert_truth(writer_position < BUFF_LEN && "Prevented buffer overflow");
    buffer[writer_position++] = c;
}

void format::appendToBuffer(const char* s) {
    while (*s != '\0') {
        assert_truth(writer_position < BUFF_LEN && "Prevented buffer overflow");
        buffer[writer_position++] = *s++;
    }
}
};  // namespace firefly
