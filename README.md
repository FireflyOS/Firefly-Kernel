# Firefly-Kernel
Kernel for FireflyOS

## x64 demo:
![Firefly OS](docs/x64-progress.png)

## Dependencies ##

 * NASM
 * xorriso
 * qemu
 * mtools
 * clang++

### Clone the repo 
 * git clone https://github.com/FireflyOS/Firefly-Kernel --recursive
 * cd Firefly-Kernel

## Ubuntu 

```bash
sudo apt install nasm xorriso qemu-system-x86 mtools clang grub
```

```bash
# ARCH can be changed accordingly to fit the architecture you need.
# To see a list of available architectures run "make target_archs"
# without the double qoutes ("")
make ARCH=x86_64
make run ARCH=x86_64
```

## Windows (WSL)

If you are on windows, you first need to install WSL. After that, you can install the dependencies by running 

```
sudo apt-get install nasm xorriso mtools clang lld grub2-common grub-pc-bin
```

You will also need to install QEMU separately. You can get it [here](https://www.qemu.org/download/).

Lastly, type `make ARCH=x86_64`, and then `make windows ARCH=x86_64`.

Alternatively, you can download QEMU with `sudo apt-get install qemu-system-x86` and use it by forwarding your xserver. More details [here](https://stackoverflow.com/questions/61110603/how-to-set-up-working-x11-forwarding-on-wsl2).

