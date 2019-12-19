# Firefly-Kernel
Kernel for FireflyOS

![Firefly OS](docs/progress.png)

## Dependencies ##
 * NASM
 * qemu
 * clang++
 * [ia16-elf-gcc](https://sourcery.mentor.com/GNUToolchain/release3298) Download the recommended package, give it executable permission, run the installer,
    and ensure its bin directory is in your PATH.

## Run ##
 * git clone https://github.com/FireflyOS/Firefly-Kernel
 * cd Firefly-Kernel
 * git submodule update --init --recursive

For the first run and when needing to start with an empty image, run `make new`.
For all other runs where the file system must be preserved (besides the kernel binaries),
run `make`.
