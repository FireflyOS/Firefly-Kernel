bits 16

global smp_trampoline_start
smp_trampoline_start:
cli
cld

bits 32
.start32:


bits 64

smp_trampoline_end:

global smp_trampoline_size
smp_trampoline_size dq smp_trampoline_end - smp_trampoline_start
