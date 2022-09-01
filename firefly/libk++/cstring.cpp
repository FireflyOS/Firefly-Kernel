#include "libk++/cstring.hpp"

namespace firefly::libkern::cstring {
size_t strlen(const char *str) {
    size_t n = 0;
    while (*str++)
        n++;
    return n;
}

int strcmp(const char *s1, const char *s2) {
    int index = 0;
    while (s1[index] && s2[index] && s1[index] == s2[index])
        index++;

    if (s1[index] == '\0' && s2[index] == '\0')
        return 0;
    else
        return s1[index] - s2[index];
}

int strncmp(const char *str1, const char *str2, size_t n) {
    for (size_t i = 0; i < n && str1; i++) {
        if (str1[i] != str2[i])
            return -1;
    }
    return 0;
}

char *strcpy(char *dest, const char *src) {
    char *ret = dest;

    int i = 0;
    do {
        dest[i] = src[i];
        i++;
    } while (src[i] != '\0');
    dest[i] = src[i];

    return ret;
}

char *strchr(const char *str, int c) {
    for (int i = 0; str[i]; i++)
        if (str[i] == (char)c)
            return (char *)&str[i];
    return nullptr;
}

char *strchrn(const char *str, int c, int n) {
    for (int i = 0; str[i] && n--; i++)
        if (str[i] == (char)c)
            return (char *)&str[i];
    return nullptr;
}

int toupper(char c) {
    if (c >= 'a' && c <= 'z')
        return c - 32;
    else
        return c;
}

char *strtok(char *s, const char *delimiters) {
    static char *next = nullptr;

    if (s != nullptr)
        next = s;

    if (next == nullptr || *next == '\0')
        return nullptr;

    char *c = next;
    while (*c != '\0') {
        for (const char *comp = delimiters; *comp != '\0'; comp++) {
            if (*comp == *c) {
                char *token = next;
                next = c + 1;
                *c = '\0';
                return token;
            }
        }

        c++;
    }

    char *token = next;
    next = c;
    return token;
}
}  // namespace firefly::libkern::cstring