bits 64

global __trampoline_start
global __trampoline_end
__trampoline_start:
	incbin "../firefly/kernel/intel64/cpu/trampoline.o"
__trampoline_end:

global __trampoline_size
__trampoline_size: dq __trampoline_end - __trampoline_start
