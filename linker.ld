ENTRY(start)

SECTIONS {
    . = 1M;

    .boot :
    {
        *(.multiboot_header)
    }

    .ctors : {
        KEEP (*crtbegin.o(.ctors))
        KEEP (*crtbegin?.o(.ctors))
        KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*(.ctors))
    }

    .dtors :
    {
        KEEP (*crtbegin.o(.dtors))
        KEEP (*crtbegin?.o(.dtors))
        KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .dtors))
        KEEP (*(SORT(.dtors.*)))
        KEEP (*(.dtors))
    }

    .text :
    {
        *(.init_start)
        *(.init)
        *(.init_end)

        *(.fini_start)
        *(.fini)
        *(.fini_end)

        *(.text*)
    }

    .data :
    {
		*(.data)
	}

    .rodata :
    {
        KEEP(*(.init_array))
        KEEP(*(.fini_array))
        *(.rodata*)
        
    }

    .bss :
    {
        *(.bss*)
    }

    /DISCARD/ :
    {
        *(*)
    }

}