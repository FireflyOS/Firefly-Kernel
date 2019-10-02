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
		*(.data)
		*(.rodata*)
		*(.bss*)
	}
    /DISCARD/ :
    {
        *(*)
    }

}