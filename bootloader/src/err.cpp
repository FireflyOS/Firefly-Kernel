#include <stdio.hpp>

/**
 *                      Prints an error message and hangs.
 */
[[noreturn]] void err(const char *msg) {
    printf(msg);

    while (1)
        ;
}