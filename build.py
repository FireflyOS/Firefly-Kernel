import os
import time
import platform

import colorama

import building.booter
import building.kernel

if platform.system() != "Linux":
    colorama.init(convert=True)

begin = time.time()

if platform.system() != "Linux":
    colorama.init(convert=True)

build_list = [
    ("kernel", building.kernel.build),
    ("bootloader", building.booter.build)
]

post_build = []

for name, build_task in build_list:
    if build_task():
        continue
    print(f"{colorama.Fore.RED}Failed to build FireflyOS on step {name}{colorama.Fore.RESET}")
    exit()

for func in post_build:
    func[0](*func[1])

print(f"{colorama.Fore.GREEN}Build finished in {round(time.time() - begin, 2)} seconds{colorama.Fore.RESET}")
