**Fork of FireflyOS**

This is indev branch, where I commiting some uncompleted things. **There is no guarantee that OS can be builded!**

# Firefly-Kernel
Kernel for Firefly
[![C/C++ CI](https://github.com/SergeyMC9730/Firefly-Kernel/actions/workflows/c-cpp.yml/badge.svg?branch=indev)](https://github.com/SergeyMC9730/Firefly-Kernel/actions/workflows/c-cpp.yml)
**Firefly OS requires 128 MB RAM**

## x64 demo:
![Firefly](docs/elbrainfuck.png)

### Clone the repo 
 * `git clone https://github.com/SergeyMC9730/Firefly-Kernel --recursive`
 * `cd Firefly-Kernel`

## Ubuntu 

```bash
sudo apt install nasm xorriso qemu-system-x86 clang lld ovmf #For UEFI emulation only
```

```bash
# ARCH can be changed accordingly to fit the architecture you need.
# To see a list of available architectures run "make target_archs"
# without the double qoutes ("")
make ARCH=x86_64
make uefi ARCH=x86_64
```
Note: Firefly can be booted on both bios and UEFI, change `uefi` to `bios` to boot it in qemu with bios


## Windows (WSL)

If you are on windows, you first need to install WSL. After that, you can install the dependencies by running 

```
sudo apt-get install nasm xorriso clang lld ovmf #For UEFI emulation only
```

You will also need to install QEMU separately. You can get it [here](https://www.qemu.org/download/).

Lastly, type `make ARCH=x86_64`, and then `make windows ARCH=x86_64`.

Alternatively, you can download QEMU with `sudo apt-get install qemu-system-x86` and use it by forwarding your xserver. More details [here](https://stackoverflow.com/questions/61110603/how-to-set-up-working-x11-forwarding-on-wsl2).

