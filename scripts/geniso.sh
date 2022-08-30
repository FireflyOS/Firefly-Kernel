#!/bin/bash
cd ..
rm -rf iso_root
mkdir iso_root/

cp limine/limine.sys limine/limine-cd.bin limine/limine-cd-efi.bin limine.cfg docs/firefly.bmp \
	build/kernel_x86_64.elf iso_root/

xorriso -as mkisofs -b limine-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot limine-cd-efi.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		iso_root -o FireflyOS_x86_64.iso

limine/limine-deploy FireflyOS_x86_64.iso
rm -rf iso_root