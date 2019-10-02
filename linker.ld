ENTRY(start)

SECTIONS {
    . = 1M;

    .boot :
    {
        *(.multiboot_header)
    }

    .text :
    {
        *(.text)
    }

    .data :
    {
		*(.data)
		*(.rodata*)
		*(.bss*)
	}

    /* warn if a section is unaccounted for */
    /DISCARD/ :
    {
		*(*)
	}
}