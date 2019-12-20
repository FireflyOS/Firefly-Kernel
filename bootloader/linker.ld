SECTIONS {
	. = 0x1400;

	.start :
	{
		*(.start)
	}

	.bentry :
	{
		*(.bentry)
	}

	.text :
	{
		*(.text*)
	}

	.data :
    {
        *(.data*)
    }

    .rodata :
    {
        *(.rodata*)
        _init_array_start = .;
        SHORT(0)
        KEEP(*(SORT(.ctors)))
        KEEP(*(.ctors))
        _init_array_end = .;
    }

    .bss :
    {
        *(COMMON)
        *(.bss*)
    }
}
