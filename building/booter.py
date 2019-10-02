import os
import shutil
import platform
import colorama


nasm_path = "nasm"

commands = [
    f"{nasm_path} -f elf64 ./booter/boot.asm",
    f"{nasm_path} -f elf64 ./booter/multiboot_header.asm",
    "ld -n -o ./binaries/boot/kernel.bin -T ./booter/linker.ld ./booter/multiboot_header.o ./booter/boot.o",
    "grub-mkrescue -o FireflyOS.iso binaries"
]

post_build = [
    (os.remove, ["./booter/multiboot_header.o"], ),
    (os.remove, ["./booter/boot.o"], ),
    (shutil.move, ["FireflyOS.iso", "binaries/boot/FireflyOS.iso"])
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

    for func in post_build:
        func[0](*func[1])

    print(f"{colorama.Fore.GREEN}Building bootloader finished!{colorama.Fore.RESET}")
    return True


if __name__ == "__main__":
    if platform.system() != "Linux":
        colorama.init(convert=True)
    build()
