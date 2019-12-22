add-symbol-file kernel/build/kernel.elf 0xffffffff80100000
break *0xffffffff80100000
target remote | qemu-system-x86_64 -m 512M -gdb stdio -drive format=raw,file=fireflyos.bin
layout src
layout regs
c
focus cmd