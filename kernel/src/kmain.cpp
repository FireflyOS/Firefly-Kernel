extern "C" void __attribute__((section(".kmain"), noreturn)) kmain() {
    *(char *)0xb8000 = 'F';
    while (true)
        ;
}