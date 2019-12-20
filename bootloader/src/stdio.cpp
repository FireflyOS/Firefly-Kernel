#include <stdarg.hpp>
#include <stdio.hpp>
#include <string.hpp>

/**
 * @brief		prints a character
 *			places a character at the current cursor location
 * @param ch		the character to print
 */

// TODO write this function in assembly

void putchar(const char ch) {
    asm volatile(
        "int $0x10\n"
        :
        : "a"(0x0e00 | ch));
}

/**
 * @brief		prints a string
 *			places characters pointed to by str
 *			onto screen at current cursor location
 * @param str		pointer to null-terminated string to print
 */
void puts(const char *str) {
    while (*str) {
        putchar(*str);
        str++;
    }
}

/**
 * @brief		prints up to n characters of a string
 *			places characters pointed to by str
 *			onto screen at current cursor location
 * @param str		pointer to null-terminated string to print
 * @param n		maximum number of characters to print
 */
void putsn(const char *str, int n) {
    while (*str && n--) {
        putchar(*str);
        str++;
    }
}

/**
 * @brief		print formatted string
 * @param fmt		the format to print
 * @param ...		data to print
 * @return		number of characters printed
 *			or -1 on error
 */
int printf(const char *fmt, ...) {
    int count = 0;

    int i;
    char *s;

    va_list args;

    const char *traverse;  // missing '}'

    va_start(args, fmt);

    for (traverse = fmt; *traverse != 0; traverse++) {
        while (*traverse != '%') {
            if (!*traverse)
                return count;
            count++;
            putchar(*traverse);
            traverse++;
        }

        traverse++;

        switch (*traverse) {
            case 'c':
                i = va_arg(args, int);
                count++;
                putchar((char)i);
                break;
            case 'i':
            case 'd':
                i = va_arg(args, int);
                if (i < 0) {
                    i = -i;
                    putchar('-');
                    count++;
                }
                s = itos(i, 10);
                count += strlen(s);
                puts(s);
                break;
            case 'o':
                i = va_arg(args, unsigned int);
                s = itos(i, 8);
                count += strlen(s);
                puts(s);
                break;
            case 's':
                s = va_arg(args, char *);
                count += strlen(s);
                puts(s);
                break;
            case 'x':
                i = va_arg(args, unsigned int);
                s = itos(i, 16);
                count += strlen(s);
                puts(s);
                break;
            default:
                return -1;
        }
    }
    return count;
}

/**
 * @brief		converts an int to a a string
 * @param num		number to convert
 8 @param base		base to convert to
 * @return		string made from num
 *			note that it will be overwritten
 *			each time this function is called
 */
char *itos(int num, int base) {
    static char intChars[] = "0123456789ABCDEF";
    static char buffer[11];
    char *ptr = buffer + 14;

    *ptr = 0;

    do {
        *--ptr = intChars[num % base];
        num /= base;
    } while (num != 0);

    return ptr;
}

/**
 * @brief		parses a base 10 int from a string
 * @param str		the string to parse
 * @return		the int parsed from str
 */
int atoi(const char *str) {
    int ret = 0;
    int i;

    for (i = 0; str[i]; i++) {
        ret = ret * 10 + str[i] - '0';
    }

    return ret;
}
