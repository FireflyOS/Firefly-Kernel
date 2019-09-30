import os
import platform
import colorama

if platform.system() != "Linux":
    colorama.init(convert=True)

nasm_path = "nasm"

commands = [
    f"{nasm_path} -f elf64 ./kernel/boot.asm",
    f"{nasm_path} -f elf64 ./kernel/multiboot",
    "ld -n -o kernel.bin -T linker.ld multiboot_header.o boot.o"    
]

def build() -> bool:
    """
    Builds the bootloader
    """
    for command in commands:
        if os.system(command):
            print(f"{colorama.Fore.RED}Building the bootloader failed at command:\n{colorama.Fore.GREEN}{command}{colorama.Fore.RESET}")
            return False
    return True

if __name__ == "__main__":
    build()