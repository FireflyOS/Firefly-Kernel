-include flags.mk

TARGET = $(BUILD_DIR)/kernel_$(ARCH).elf
ISO = FireflyOS_$(ARCH).iso


all: $(TARGET)

$(TARGET): $(CONV_FILES)
	ld.lld -o $@ --no-undefined -T $(SOURCE_DIR)/linkage/linker_$(ARCH).ld -nostdlib -m elf_$(ARCH) $(OBJ_FILES) $(LIB_OBJS) 
	cp linkage/multi_arch_grub/grub.$(ARCH) binaries/boot/grub/grub.cfg
	grub-mkrescue -o FireflyOS_$(ARCH).iso binaries
	
target_archs:
	@printf "Supported architectures:\n";
	@printf "x86_64\n";
	@printf "i386\n";
	@printf "\n"

clean:
	rm -rf binaries/boot/arch
	rm binaries/boot/kernel_i386.elf || echo ""
	rm binaries/boot/kernel_x86_64.elf || echo ""
	rm include/stl/stdio.o include/stl/cstd.o

run:
	cp linkage/multi_arch_grub/grub.$(ARCH) binaries/boot/grub/grub.cfg
	qemu-system-$(ARCH) -M q35 -m 256M -boot d -no-shutdown -serial stdio -no-reboot -cdrom $(ISO) $(QEMU_FLAGS) -d int

debug: $(ISO) $(TARGET)
	cp linkage/multi_arch_grub/grub.$(ARCH) binaries/boot/grub/grub.cfg
	qemu-system-$(ARCH) -M q35 -m 256M -boot d -cdrom $(ISO) $(QEMU_FLAGS) -S -s -monitor stdio

%.asm.o: %.asm
	$(AS) $< $(ASM_FLAGS) -o $(BUILD_DIR)/$@
