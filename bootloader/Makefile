BUILD_DIR = build
SRC_DIR = src
INC_DIR = include
LIB_DIR = dependencies
OUTPUT = bootloader.bin

CXX_FLAGS = -I$(INC_DIR)/ -c -m16 -fno-pie -Wall -Wextra -pedantic -std=c++17 -nostdlib -nostartfiles -ffreestanding -fno-builtin -fno-PIC -fno-stack-check -fno-stack-protector -fno-omit-frame-pointer -fno-exceptions -fno-rtti
CXX_OPT_FLAG := -Os

LIBRARIES =

CXX_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(CXX_FILES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%_cxx.o)
ASM_FILES = $(wildcard $(SRC_DIR)/*.asm)
OBJ_FILES += $(ASM_FILES:$(SRC_DIR)/%.asm=$(BUILD_DIR)/%_asm.o)

# build and run code preserving file system
all: $(OUTPUT)

clean:
	rm $(BUILD_DIR)/*

$(BUILD_DIR)/mbr.bin: $(SRC_DIR)/boot/mbr.asm
	nasm -f bin -o $(BUILD_DIR)/mbr.bin $(SRC_DIR)/boot/mbr.asm

$(BUILD_DIR)/vbr.bin: $(SRC_DIR)/boot/vbr.asm
	nasm -f bin -o $(BUILD_DIR)/vbr.bin $(SRC_DIR)/boot/vbr.asm

# compile C and asm files into objects
$(BUILD_DIR)/%_cxx.o: $(SRC_DIR)/%.cpp
	g++ -o $@ $(CXX_FLAGS) $(CXX_OPT_FLAG) $<

$(BUILD_DIR)/%_asm.o: $(SRC_DIR)/%.asm
	nasm -o $@ -f elf32 $<
# test
$(OUTPUT): $(BUILD_DIR)/vbr.bin $(BUILD_DIR)/mbr.bin $(OBJ_FILES)
	ld --no-undefined -m elf_i386 -T linker.ld -o $(BUILD_DIR)/bootloader.elf $(OBJ_FILES)
	objcopy -O binary --set-section-flags .bss=alloc,load,contents $(BUILD_DIR)/bootloader.elf bootloader.bin
