SRC_DIR = src
INC_DIR = include
BUILD_DIR = build

QEMU_BP := kernel_main
QEMU_FLAGS :=

CXX_FLAGS = -I./include -I./include/stl -target x86_64-unknown-elf -m64 -mcmodel=kernel -std=c++17 -Wall -Wextra -pedantic -Werror -g -O2 -nostdlib -fno-builtin -fno-PIC -mno-red-zone -fno-stack-check -fno-stack-protector -fno-omit-frame-pointer -ffreestanding -fno-exceptions -fno-rtti

LIB_OBJS = ./include/stl/cstd.o

# source files
CXX_ROOT_FILES = $(wildcard $(SRC_DIR)/*.cpp)
CXX_DRIVER_FILES = $(wildcard $(SRC_DIR)/drivers/*.cpp)
CXX_FILES = $(CXX_ROOT_FILES) $(CXX_DRIVER_FILES)
ASM_FILES = $(wildcard $(SRC_DIR)/*.asm)

# object files
ROOT_OBJ_FILES = $(CXX_ROOT_FILES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%_cxx.o)
DRIVER_OBJ_FILES = $(CXX_DRIVER_FILES:$(SRC_DIR)/drivers/%.cpp=$(BUILD_DIR)/drivers/%_cxx.o)
ASM_OBJ_FILES = $(ASM_FILES:$(SRC_DIR)/%.asm=$(BUILD_DIR)/%_asm.o)
OBJ_FILES = $(ROOT_OBJ_FILES) $(DRIVER_OBJ_FILES) $(ASM_OBJ_FILES) $(LIB_OBJS)

all: create_dirs build

create_dirs:
	mkdir -p ./build/drivers

build: kernel.bin

./include/stl/cstd.o:
	make -C ./include/stl

clean:
	rm $(BUILD_DIR)/*.o
	rm $(BUILD_DIR)/*.bin
	rm include/stl/cstd.o

# build objects
$(BUILD_DIR)/%_cxx.o: $(SRC_DIR)/%.cpp
	clang++ $(CXX_FLAGS) -c $< -o $@

$(BUILD_DIR)/drivers/%_cxx.o: $(SRC_DIR)/drivers/%.cpp
	clang++ $(CXX_FLAGS) -c $< -o $@

$(BUILD_DIR)/%_asm.o: $(SRC_DIR)/%.asm
	nasm $< -f elf64 -o $@

kernel.bin: $(OBJ_FILES)
	ld -o $(BUILD_DIR)/kernel.elf -m elf_x86_64 --no-undefined -T linker.ld \
		-nostdlib $(OBJ_FILES)
	objcopy -O binary --set-section-flags .bss=alloc,load,content $(BUILD_DIR)/kernel.elf kernel.bin