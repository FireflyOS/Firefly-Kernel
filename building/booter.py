import os
import shutil
import platform
import colorama


nasm_path = "nasm"
linker_path = "ld.lld"

commands = [
    f"{nasm_path} -f elf64 ./booter/boot.asm -o ./binaries/boot/boot.o",
    f"ld -n -o ./binaries/boot/boot.o ./binaries/boot/kernel.o",
    f"{nasm_path} -f elf64 ./booter/multiboot_header.asm -o ./binaries/boot/multiboot_header.o",
    "ld -n -o ./binaries/boot/kernel.bin -T ./booter/linker.ld ./binaries/boot/multiboot_header.o ./binaries/boot/boot.o ./binaries/boot/kernel.o",
    "grub-mkrescue -o FireflyOS.iso binaries"
]

def build() -> bool:
    """
    Builds the bootloader
    """
    if not os.path.isdir("./binaries/boot"):
        os.makedirs("./binaries/boot")

    for command in commands:
        if os.system(command):
            print(f"{colorama.Fore.RED}Building the bootloader failed at command:\n{colorama.Fore.GREEN}{command}{colorama.Fore.RESET}")
            return False

    print(f"{colorama.Fore.GREEN}Building bootloader finished!{colorama.Fore.RESET}")
    return True


if __name__ == "__main__":
    if platform.system() != "Linux":
        colorama.init(convert=True)
    build()
