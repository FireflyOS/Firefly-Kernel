#include <stdio.hpp>

/**
 *                      Prints an error message and hangs.
 */
void __attribute__((noreturn)) err(const char *msg) {
    printf(msg);

    while (1)
        ;
}