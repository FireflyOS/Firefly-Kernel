# Firefly-Kernel
Kernel for FireflyOS

![Firefly OS](docs/progress.png)

## Dependencies ##
 * NASM
 * qemu
 * clang++
 * g++

## Run ##
 * git clone https://github.com/FireflyOS/Firefly-Kernel
 * cd Firefly-Kernel
 * git submodule update --init --recursive

For the first run and when needing to start with an empty image, run `make new`.
For all other runs where the file system must be preserved (besides the kernel binaries),
run `make`.
