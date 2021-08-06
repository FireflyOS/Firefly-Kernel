OUTPUT_FORMAT(elf32-i386)
ENTRY(start)

SECTIONS {
    . = 0x00100000;

    /* this is all used before mapping
       so give its labels physical addresses */
    .pm_stub :
    {
        *(.multiboot_header)
        *(.pm_stub)
    }

    . = ALIGN(4096);

    /* virtual start address */
    _kernel_start = .;
    .text ALIGN (4096) : AT (ADDR (.text))
    {
        *(.text*)
    }

    .data ALIGN (4096) : AT (ADDR (.data))
    {
        /* *(.eh_frame)
        *(.eh_frame_hdr) */
        *(.data*)
    }

    .rodata ALIGN (4096) : AT (ADDR (.rodata))
    {
        *(.rodata*)
        _init_array_start = .;
        KEEP(*(SORT(.ctors)))
        KEEP(*(.ctors))
        _init_array_end = .;
    }

    .bss ALIGN (4096) : AT (ADDR (.bss))
    {
        *(COMMON)
        *(.bss*)
    }
    /* physical end address */
    _kernel_end = ALIGN(4096);
}