ENTRY(start)

VIRT_BASE = 0xFFFFFFFF80000000;

SECTIONS {
    . = 0x00100000;

    /* this is all used before mapping
       so give its labels physical addresses */
    .pm_stub :
    {
        *(.multiboot_header)
        *(.pm_stub)
    }

    . += VIRT_BASE;
    . = ALIGN(4096);

    /* virtual start address */
    _kernel_start = .;
    .text ALIGN (4096) : AT (ADDR (.text) - VIRT_BASE)
    {
        *(.text*)
    }

    .data ALIGN (4096) : AT (ADDR (.data) - VIRT_BASE)
    {
        /* *(.eh_frame)
        *(.eh_frame_hdr) */
        *(.data*)
    }

    .rodata ALIGN (4096) : AT (ADDR (.rodata) - VIRT_BASE)
    {
        *(.rodata*)
        init_array_start = .;
        KEEP(*(SORT(.ctors)))
        KEEP(*(.ctors))
        init_array_end = .;
    }

    .bss ALIGN (4096) : AT (ADDR (.bss) - VIRT_BASE)
    {
        *(COMMON)
        *(.bss*)
    }
    /* physical end address */
    _kernel_end = ALIGN(4096);

    /* ASSERT(((_kernel_end - _kernel_start) < 0x200000), "Error: Kernel larger than 2MiB"); */
}