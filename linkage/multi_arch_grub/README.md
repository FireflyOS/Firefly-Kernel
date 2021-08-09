### Grub loader/

The Makefile will enter this folder and use the grub file with the corresponding architecture as a grub.cfg in `binaries/boot/grub`

This way grub is told to boot kernel_{ARCHITECTURE}.elf, allowing us to have elf files named `kernel_{ARCHITECTURE}.elf` instead of a static name.

This process is fully automatic, the only rules are as follows:

- The file **must** have the same extension as the `ARCH` argument passed to make, and begin with grub (i.e. `grub.ARCH`)
- The only content you need to change is:
```
multiboot2 /boot/kernel_{TARGET_ARCHITECTURE}.elf
```
where `TARGET_ARCHITECTURE` is what you passed as an argument to make. Exclude the curly braces (`{}`) (`make all ARCH=i386`~i386)

Example:
```python
set timeout=0
set default=0

menuentry "CursedFirefly" {
    multiboot2 /boot/kernel_i386.elf
    boot
}
```