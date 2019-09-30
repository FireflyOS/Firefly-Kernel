import platform

import colorama

import building.booter

if platform.system() != "Linux":
    colorama.init(convert=True)

build_dict = {
    "bootloader": building.booter.build
}

for name, build_task in build_dict.items():
    if not build_task():

        print(f"{colorama.Fore.RED}Failed to build FireflyOS on step {name}{colorama.Fore.RESET}")
