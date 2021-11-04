#!/bin/bash
cd ..
rm -rf iso_root
mkdir -p iso_root/boot

cp limine/BOOTIA32.EFI limine/BOOTX64.EFI limine/limine.sys limine/limine-cd.bin \
	limine/limine-eltorito-efi.bin limine/limine-pxe.bin limine.cfg \
	build/kernel_x86_64.elf iso_root/boot

xorriso -as mkisofs -b boot/limine-cd.bin \
	-no-emul-boot -boot-load-size 4 -boot-info-table \
	--efi-boot boot/limine-eltorito-efi.bin -efi-boot-part \
	--efi-boot-image --protective-msdos-label iso_root -o kernel.iso

limine/limine-install kernel.iso
rm -rf iso_root
rm Scripts/parsed_x86_64.sym