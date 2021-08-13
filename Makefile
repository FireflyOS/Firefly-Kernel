-include flags.mk

TARGET = $(BUILD_DIR)/kernel_$(ARCH).elf
ISO = FireflyOS_$(ARCH).iso


all: create_dirs $(TARGET)

$(TARGET): $(CONV_FILES)
	$(MAKE) -C ./include/stl # Build STL before linking
	ld.lld -o $@ --no-undefined -T linkage/linker_$(ARCH).ld -nostdlib -m elf_$(ARCH) $(OBJ_FILES) $(LIB_OBJS) 
	cp linkage/multi_arch_grub/grub.$(ARCH) binaries/boot/grub/grub.cfg
	grub-mkrescue -o FireflyOS_$(ARCH).iso binaries
	
# TODO: Find a better way to copy the folder structure of arch/{arch}/ into binaries/boot
create_dirs:
ifeq ($(ARCH), x86_64)
	mkdir -vp $(BUILD_DIR)/arch/$(ARCH)/kernel/drivers
	mkdir -vp $(BUILD_DIR)/arch/$(ARCH)/kernel/init
	mkdir -vp $(BUILD_DIR)/arch/$(ARCH)/kernel/int
	mkdir -vp $(BUILD_DIR)/arch/$(ARCH)/libk++
endif
ifeq ($(ARCH), i386)
	mkdir -vp $(BUILD_DIR)/arch/$(ARCH)/kernel/drivers
	mkdir -vp $(BUILD_DIR)/arch/$(ARCH)/kernel/trace
	mkdir -vp $(BUILD_DIR)/arch/$(ARCH)/kernel/init
	mkdir -vp $(BUILD_DIR)/arch/$(ARCH)/kernel/int
	mkdir -vp $(BUILD_DIR)/arch/$(ARCH)/kernel/gdt
	mkdir -vp $(BUILD_DIR)/arch/$(ARCH)/libk++
endif

target_archs:
	@printf "Supported architectures:\n";
	@printf "x86_64 (Encouraged)\n";
	@printf "i386 (Very-WIP)\n";
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


%.cxx.o: %.cpp
	$(CC) $(CXX_FLAGS) -c $< -o $(BUILD_DIR)/$@

%.asm.o: %.asm
	$(AS) $< $(ASM_FLAGS) -o $(BUILD_DIR)/$@
