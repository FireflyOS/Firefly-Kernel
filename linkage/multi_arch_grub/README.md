### Grub loader/

The Makefile will come into this folder in order to use it as a grub.cfg.

This way grub is told to boot kernel_{ARCHITECTURE}.elf, allowing us to have elf files named `kernel_{ARCHITECTURE}.elf` instead of a static name.

This process is fully automatic, the only rules are as follows:

- The file **must** have the same extension as the `ARCH` argument passed to make, and begin with grub (i.e. `grub.ARCH`)
- The only thing you should need to change is:
```
multiboot2 /boot/kernel_{TARGET_ARCHITECTURE}.elf
```
where `TARGET_ARCHITECTURE` is what you passed as an argument to make. Exclude the curly braces (`{}`) (`make all ARCH=i386`~i386)