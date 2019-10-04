ENTRY(start)

SECTIONS {
    . = 1M;

    .boot :
    {
        *(.multiboot_header)
    }

    /* Include the list of initialization functions sorted. */
    /* .init_array :
    {
        crti.o(.init_array)
        KEEP (*(SORT(EXCLUDE_FILE(crti.o crtn.o) .init_array.*)))
        KEEP (*(EXCLUDE_FILE(crti.o crtn.o) .init_array))
        crtn.o(.init_array)
    } */

    /* Include the list of termination functions sorted. */
    /* .fini_array :
    {
        crti.o(.fini_array)
        KEEP (*(SORT(EXCLUDE_FILE(crti.o crtn.o) .fini_array.*)))
        KEEP (*(EXCLUDE_FILE(crti.o crtn.o) .fini_array))
        crtn.o(.fini_array)
    } */

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