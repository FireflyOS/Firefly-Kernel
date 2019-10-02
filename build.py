import time
import platform

import colorama

import building.booter

if platform.system() != "Linux":
    colorama.init(convert=True)

begin = time.time()

if platform.system() != "Linux":
    colorama.init(convert=True)

build_dict = {
    "bootloader": building.booter.build
}

for name, build_task in build_dict.items():
    if build_task():
        continue
    print(f"{colorama.Fore.RED}Failed to build FireflyOS on step {name}{colorama.Fore.RESET}")
    exit()

print(f"{colorama.Fore.GREEN}Build finished in {round(time.time() - begin, 2)} seconds{colorama.Fore.RESET}")