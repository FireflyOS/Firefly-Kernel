org 0x2000
bits 16

cli

.hang:
		hlt
	jmp .hang
