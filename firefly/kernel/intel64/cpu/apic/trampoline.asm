bits 16

section .rodata

global smp_trampoline_start
smp_trampoline_start:
cli
cld
mov dx, 0xE9
mov al, 'e'
out dx, al

smp_trampoline_end:

global smp_trampoline_size
smp_trampoline_size dq smp_trampoline_end - smp_trampoline_start
