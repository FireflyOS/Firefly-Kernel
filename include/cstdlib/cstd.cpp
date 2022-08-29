#include <cstddef>
#include <cstdint>

// void *memset(void *__restrict dest, int value, size_t count) {
//     auto _ptr = reinterpret_cast<char *>(dest);
//     for (size_t i = 0; i < count; _ptr[i++] = value)
//         ;
//     return dest;
// }

// void *memcpy(void *__restrict dest, const void *src, size_t count) {
//     // cast pointers to char*,
//     // since void pointers are not suitable for arithmetics
//     char *csrc = (char *)src;
//     char *cdest = (char *)dest;

//     for (size_t i = 0; i < count; i++) {
//         cdest[i] = csrc[i];
//     }

//     return dest;
// }

/**
 * @brief		compares n bytes of two blocks of memory
 * @param s1		the first block
 * @param s2		the second block
 * @param n		amount of bytes to compare
 * @return		comparison of the blocks' values
 *			0   : the blocks are equal
 *			< 0 : s1 comes before s2 numerically
 *			> 0 : s2 comes before s1 numerically
 */
int memcmp(const char *s1, const char *s2, int n) {
    int index = 0;
    while (s1[index] && s2[index] && s1[index] == s2[index] && n--)
        index++;

    if (n == 0)
        return 0;
    else
        return s1[index] - s2[index];
}

/**
 * @brief		compares two strings
 * @param s1		the first string
 * @param s2		the second string
 * @return		comparison of the strings' values
 *			0   : the strings are equal
 *			< 0 : s1 comes before s2 alphabetically
 *			> 0 : s2 comes before s1 alphabetically
 */
int strcmp(const char *s1, const char *s2) {
    int index = 0;
    while (s1[index] && s2[index] && s1[index] == s2[index])
        index++;

    if (s1[index] == '\0' && s2[index] == '\0')
        return 0;
    else
        return s1[index] - s2[index];
}

/**
 * @brief		copies the string str into dest
 * @param dest		destination of string
 * @param str		source string
 * @return		dest
 */
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


/**
 * @brief		searches for a character in a string
 * @param str		the string to search through
 * @param c		the character to search for
 * @return		pointer to character in string if found
 *			otherwise NULL
 */
char *strchr(const char *str, int c) {
    for (int i = 0; str[i]; i++)
        if (str[i] == (char)c)
            return (char *)&str[i];
    return nullptr;
}

/**
 * @brief		searches for a character in a string
 * @param str		the string to search through
 * @param c		the character to search for
 * @param n		max number of chars to compare
 * @return		pointer to character in string if found
 *			otherwise NULL
 */
char *strchrn(const char *str, int c, int n) {
    for (int i = 0; str[i] && n--; i++)
        if (str[i] == (char)c)
            return (char *)&str[i];
    return nullptr;
}

/**
 * @brief		converts a character to uppercase
 * @param c		the character to convert
 * @return		c convertred to uppercase
 *			c      : c is not a lowercase letter
 *			c + 32 : c is a lowercase letter
 */
int toupper(char c) {
    if (c >= 'a' && c <= 'z')
        return c - 32;
    else
        return c;
}

/**
 * @brief		tokenizes a string
 * @param s		pointer to string to tokenize
 *			or NULL to continue tokenizing string
 * @param delimiters	list of chars by which to split s
 * @return		pointer to last created token in s
 *			or NULL if s is empty
 */
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

int digitcount(uint32_t num) {
    int res = 0;
    while (num != 0) {
        ++res;
        num /= 10;
    }
    return res;
}