#include <stdio.hpp>

void __attribute__((noreturn)) err(const char *msg) {
    printf(msg);

    while (1)
        ;
}