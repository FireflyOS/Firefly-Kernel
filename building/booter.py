import os
import shutil
import platform
import colorama

if platform.system() != "Linux":
    colorama.init(convert=True)

nasm_path = "C:\\Users\\john_\\AppData\\Local\\bin\\NASM\\nasm.exe"

commands = [
    f"{nasm_path} -f elf64 ./booter/boot.asm",
    f"{nasm_path} -f elf64 ./booter/multiboot_header.asm",
    "ld -n -o ./binaries/bootloader/kernel.bin -T ./booter/linker.ld ./booter/multiboot_header.o ./booter/boot.o"
]

post_build = [
    (os.remove, ["./booter/multiboot_header.o"], ),
    (os.remove, ["./booter/boot.o"], ),
]


def build() -> bool:
    """
    Builds the bootloader
    """
    if not os.path.isdir("./binaries/bootloader"):
        os.makedirs("./binaries/bootloader")
    for command in commands:
        if os.system(command):
            print(f"{colorama.Fore.RED}Building the bootloader failed at command:\n{colorama.Fore.GREEN}{command}{colorama.Fore.RESET}")
            return False

    for func in post_build:
        func[0](*func[1])

    return True


if __name__ == "__main__":
    build()
