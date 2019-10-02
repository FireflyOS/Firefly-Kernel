import os
import shutil
import platform
import colorama


flags = "-target x86_64-unknown-elf -m64 -std=c++17 -Wall -Wextra -pedantic -Werror -g -O2 -fno-PIC -mno-red-zone -fno-stack-protector -fno-omit-frame-pointer -ffreestanding -fno-exceptions -fno-rtti -c"

commands = [
    f"clang++ {flags} ./kernel/kernel.cpp -o ./binaries/boot/kernel.o"
]

post_build = []

def build() -> bool:
    """
    Builds the kernel
    """
    if not os.path.isdir("./binaries/boot"):
        os.makedirs("./binaries/boot")

    for command in commands:
        if os.system(command):
            print(f"{colorama.Fore.RED}Building the kernel failed at command:\n{colorama.Fore.GREEN}{command}{colorama.Fore.RESET}")
            return False

    for func in post_build:
        func[0](*func[1])

    print(f"{colorama.Fore.GREEN}Building kernel finished!{colorama.Fore.RESET}")
    return True


if __name__ == "__main__":
    if platform.system() != "Linux":
        colorama.init(convert=True)
    build()
