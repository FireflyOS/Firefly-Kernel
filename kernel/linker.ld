VIRT_BASE = 0xffffffff80000000;

SECTIONS {
    . = VIRT_BASE + 0x00100000;

    _kernel_start = .;

    .start : AT (ADDR (.start) - VIRT_BASE)
    {
        *(.start)
        *(.kmain)
    }
    
    .text : AT (ADDR (.text) - VIRT_BASE)
    {
        *(.text*)
    }

    .data ALIGN (4096) : AT (ADDR (.data) - VIRT_BASE)
    {
        *(.data*)
    }

    .rodata ALIGN (4096) : AT (ADDR (.rodata) - VIRT_BASE)
    {
        *(.rodata*)
        _init_array_start = .;
        LONG(0)
        KEEP(*(SORT(.ctors)))
        KEEP(*(.ctors))
        _init_array_end = .;
    }

    .bss ALIGN (4096) : AT (ADDR (.bss) - VIRT_BASE)
    {
        *(COMMON)
        *(.bss*)
    }

    _kernel_end = ALIGN(4096);
}