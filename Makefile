#
# Quick & dirty makefile to automate the process of typing all these commands/
#  Note: This is specific to any shell that supports ';' as a subcommand separator, it is NOT portable.
#  See: https://lists.gnu.org/archive/html/help-make/2008-05/msg00047.html
#
all:
ifeq ($(wildcard ./build),)
	make -C limine/
	meson build --cross-file meson_config.txt
endif
	cd build; \
	meson compile && \
	../scripts/geniso.sh

run:
	cd build; \
	../scripts/qemu-bios.sh

uefi:
	cd build; \
	../scripts/qemu-uefi.sh

clean:
ifeq ($(wildcard ./build),)
	$(error No build directory to clean, please run 'make all' or 'make' first)
endif
	cd build; \
	ninja clean