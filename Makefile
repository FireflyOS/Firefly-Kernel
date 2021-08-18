-include flags.mk

TARGET = $(BUILD_DIR)/kernel_$(ARCH).elf
ISO = FireflyOS_$(ARCH).iso

# test:
# 	clang++ $(CXX_FLAGS) -c arch/x86_64/kernel/kernel.cpp -o $(BUILD_DIR)/arch/x86_64/kernel/kernel.o
# 	ld.lld -o $(BUILD_DIR)/kernel_x86_64.elf -T linkage/linker_x86_64.ld -nostdlib


all: create_dirs $(ISO)

$(ISO): $(TARGET)
	$(MAKE) -C limine
	rm -rf iso_root
	mkdir -p iso_root
	cp $< limine.cfg limine/limine.sys limine/limine-cd.bin limine/limine-eltorito-efi.bin iso_root/
	xorriso -as mkisofs -b limine-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot limine-eltorito-efi.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		iso_root -o $@
	limine/limine-install $@
	rm -rf iso_root

$(TARGET): $(CONV_FILES)
	$(MAKE) -C ./include/stl # Build STL before linking
	ld.lld -o $@ --no-undefined -T linkage/linker_$(ARCH).ld -nostdlib -zmax-page-size=0x1000 -static -pie --no-dynamic-linker -ztext $(OBJ_FILES) $(LIB_OBJS) 

# TODO: Find a better way to copy the folder structure of arch/{arch}/ into binaries/boot
create_dirs:
ifeq ($(ARCH), x86_64)
	mkdir -vp $(BUILD_DIR)/arch/$(ARCH)/kernel/memory-manager/
	mkdir -vp $(BUILD_DIR)/arch/$(ARCH)/kernel/drivers
	mkdir -vp $(BUILD_DIR)/arch/$(ARCH)/kernel/init
	mkdir -vp $(BUILD_DIR)/arch/$(ARCH)/kernel/int
	mkdir -vp $(BUILD_DIR)/arch/$(ARCH)/libk++
endif

target_archs:
	@printf "Supported architectures:\n";
	@printf "x86_64 (Encouraged)\n";
	@printf "\n"

clean:
	rm -rf binaries/boot/arch
	rm binaries/boot/kernel_i386.elf || echo ""
	rm binaries/boot/kernel_x86_64.elf || echo ""
	rm include/stl/stdio.o include/stl/cstd.o

run:
# cp linkage/multi_arch_grub/grub.$(ARCH) binaries/boot/grub/grub.cfg
	qemu-system-$(ARCH) -M q35 -m 256M -boot d -no-shutdown -serial stdio -no-reboot -cdrom $(ISO) $(QEMU_FLAGS) -d int

debug: $(ISO) $(TARGET)
	cp linkage/multi_arch_grub/grub.$(ARCH) binaries/boot/grub/grub.cfg
	qemu-system-$(ARCH) -M q35 -m 256M -boot d -cdrom $(ISO) $(QEMU_FLAGS) -S -s -monitor stdio


%.cxx.o: %.cpp
	$(CC) $(CXX_FLAGS) -c $< -o $(BUILD_DIR)/$@

%.asm.o: %.asm
	$(AS) $< $(ASM_FLAGS) -o $(BUILD_DIR)/$@