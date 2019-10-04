SRC_DIR = kernel
INC_DIR = include
BUILD_DIR = binaries/boot

QEMU_BP := kernel_main
QEMU_FLAGS :=

CXX_FLAGS = -I./include -I./include/stl -target x86_64-unknown-elf -m64 -std=c++17 -Wall -Wextra -pedantic -Werror -g -O2 -nostdlib -fno-builtin -fno-PIC -mno-red-zone -fno-stack-check -fno-stack-protector -fno-omit-frame-pointer -ffreestanding -fno-exceptions -fno-rtti

# source files
CXX_ROOT_FILES = $(wildcard $(SRC_DIR)/*.cpp)
CXX_DRIVER_FILES = $(wildcard $(SRC_DIR)/drivers/*.cpp)
CXX_FILES = $(CXX_ROOT_FILES) $(CXX_DRIVER_FILES)
ASM_FILES = $(wildcard $(SRC_DIR)/*.asm)

# object files
ROOT_OBJ_FILES = $(CXX_ROOT_FILES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%_cxx.o)
DRIVER_OBJ_FILES = $(CXX_DRIVER_FILES:$(SRC_DIR)/drivers/%.cpp=$(BUILD_DIR)/drivers/%_cxx.o)
ASM_OBJ_FILES = $(ASM_FILES:$(SRC_DIR)/%.asm=$(BUILD_DIR)/%_asm.o)
OBJ_FILES = $(ROOT_OBJ_FILES) $(DRIVER_OBJ_FILES) $(ASM_OBJ_FILES)

# build objects
$(BUILD_DIR)/%_cxx.o: $(SRC_DIR)/%.cpp
	clang++ $(CXX_FLAGS) -c $< -o $@

$(BUILD_DIR)/drivers/%_cxx.o: $(SRC_DIR)/drivers/%.cpp
	clang++ $(CXX_FLAGS) -c $< -o $@

$(BUILD_DIR)/%_asm.o: $(SRC_DIR)/%.asm
	nasm $< -f elf64 -o $@

all: create_dirs build run

create_dirs:
	mkdir -p ./binaries/boot/drivers

build: $(BUILD_DIR)/kernel.bin

clean:
	rm $(BUILD_DIR)/*.o
	rm $(BUILD_DIR)/*.bin

run:
	qemu-system-x86_64 -boot d -cdrom ./FireflyOS.iso

debug: build FireflyOS.iso $(BUILD_DIR)/kernel.bin
	qemu-system-x86_64 -cdrom FireflyOS.iso $(QEMU_FLAGS) -S -s &
	gdb $(BUILD_DIR)/kernel.bin \
		-ex 'target remote localhost:1234' \
		-ex 'layout src' \
		-ex 'layout regs' \
		-ex 'break $(QEMU_BP)' \
		-ex 'continue'

$(BUILD_DIR)/kernel.bin: $(OBJ_FILES) 
	ld --no-undefined -T linker.ld -o $(BUILD_DIR)/kernel.bin $(OBJ_FILES)

	grub-mkrescue -o FireflyOS.iso binaries