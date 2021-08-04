-include flags.mk

all: $(BUILD_DIR)/kernel.elf

$(BUILD_DIR)/kernel.elf: $(CONV_FILES)
	$(MAKE) -C ./include/stl # Build STL before linking
	ld -o $@ --no-undefined -T linker.ld -nostdlib $(OBJ_FILES) $(LIB_OBJS)
	grub-mkrescue -o FireflyOS.iso binaries
	
# TODO: Find a better way to copy the folder structure of kernel/ into binaries/boot
create_dirs:
	mkdir -vp ./binaries/boot/kernel/drivers
	mkdir -vp ./binaries/boot/kernel/init
	mkdir -vp ./binaries/boot/kernel/int

clean:
	rm $(OBJ_FILES)
	rm $(BUILD_DIR)/kernel.elf
	rm $(LIB_OBJS)

run:
	qemu-system-x86_64 -M q35 -m 256M -boot d -no-shutdown -no-reboot -cdrom ./FireflyOS.iso

debug: FireflyOS.iso $(BUILD_DIR)/kernel.elf
	qemu-system-x86_64 -boot d -cdrom ./FireflyOS.iso $(QEMU_FLAGS) -S -s


%.cxx.o: %.cpp
	$(CC) $(CXX_FLAGS) -c $< -o $(BUILD_DIR)/$@

%.asm.o: %.asm
	nasm $< -f elf64 -g -F dwarf -o $(BUILD_DIR)/$@
