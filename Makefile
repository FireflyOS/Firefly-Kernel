-include flags.mk

$(BUILD_DIR)/kernel.elf: $(CONV_FILES)
	$(MAKE) -C ./include/stl # Build STL before linking
	ld -o $@ --no-undefined -T linker.ld -nostdlib $(OBJ_FILES) $(LIB_OBJS)
	grub-mkrescue -o FireflyOS.iso binaries
	qemu-system-x86_64 -M q35 -m 256M -boot d -no-shutdown -no-reboot -cdrom ./FireflyOS.iso


all: create_dirs $(BUILD_DIR)/kernel.elf

# TODO: Find a better way to copy the folder structure of kernel/ into binaries/boot
create_dirs:
	mkdir -vp ./binaries/boot/kernel/drivers init int

clean:
	rm $(OBJ_FILES)
	rm $(BUILD_DIR)/kernel.elf
	rm $(LIB_OBJS)

run:
	qemu-system-x86_64 -M q35 -m 256M -boot d -no-shutdown -no-reboot -cdrom ./FireflyOS.iso

debug: build FireflyOS.iso $(BUILD_DIR)/kernel.elf
	qemu-system-x86_64 -boot d -cdrom ./FireflyOS.iso \
	$(QEMU_FLAGS) -S -s &
	gdb $(BUILD_DIR)/kernel.elf \
		-ex 'target remote localhost:1234' \
		-ex 'layout src' \
		-ex 'layout regs' \
		-ex 'break *0x100018' \
		-ex 'continue'

%.cxx.o: %.cpp
	$(CC) $(CXX_FLAGS) -c $< -o $(BUILD_DIR)/$@

%.asm.o: %.asm
	nasm $< -f elf64 -o $(BUILD_DIR)/$@