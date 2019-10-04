ENTRY(start)

SECTIONS {
    . = 1M;

    .boot :
    {
        *(.multiboot_header)
    }

    .text :
    {
        *(.text*)
    }

    .data :
    {
        /* *(.eh_frame)
        *(.eh_frame_hdr) */
        *(.data*)
    }

    .rodata :
    {
        *(.rodata*)
        init_array_start = .;
        KEEP(*(SORT(.ctors)))
        KEEP(*(.ctors))
        init_array_end = .;
    }

    .bss :
    {
        *(COMMON)
        *(.bss*)
    }
}