add-symbol-file bootloader/build/bootloader.elf 0x1400
break *0x2737
target remote | qemu-system-x86_64 -m 512M -gdb stdio -drive format=raw,file=fireflyos.bin
c
