align 4096
bits 16

section .rodata

global smp_trampoline_start
smp_trampoline_start:
	cli
	cld

	xor ax, ax
	mov ds, ax

	lgdt [gdtr]

gdt:
gdt_null:
	dd 0
	dd 0
gdt_code:
	dw 0xFFFF
	dw 0
	db 0
	db 10011010b
	db 11001111b
        db 0
gdt_data:
	dw 0xFFFF
        dw 0
        db 0
        db 10010010b
        db 11001111b
        db 0
gdt_end:
gdtr:                           ; The GDT descriptor
        dw gdt_end - gdt - 1    ; Limit (size)
        dd gdt                  ; Address of the GDT

smp_trampoline_end:

global smp_trampoline_size
smp_trampoline_size dq smp_trampoline_end - smp_trampoline_start
