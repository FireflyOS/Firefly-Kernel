void __attribute__((noreturn)) err() {
    asm volatile(
        "int $0x10\n"
        :
        : "a"(0x0e00 | 'E'));

    while (1)
        ;
}