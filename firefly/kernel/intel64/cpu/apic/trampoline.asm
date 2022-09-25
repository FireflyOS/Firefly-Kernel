align 4096
bits 16

section .rodata

global smp_trampoline_start
smp_trampoline_start:
	cli
	cld

	xor ax, ax
	mov ds, ax
.hang:
	hlt
	jmp .hang


smp_trampoline_end:

global smp_trampoline_size
smp_trampoline_size dq smp_trampoline_end - smp_trampoline_start
