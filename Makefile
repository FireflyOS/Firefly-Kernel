OUTPUT = fireflyos.bin

# build and run code preserving file system
all: build run

# build and run code creating new image with empty file system
new: create_dirs build_new run

build: create_dirs $(OUTPUT)

build_new: $(OUTPUT)_new $(OUTPUT)

create_dirs:
	mkdir -p fireflyfs

run:
	qemu-system-x86_64 -m 512M -drive format=raw,file=$(OUTPUT)

debug: build
	gdb -command=./.gdbinit

clean:
	rm -f bootloader/build/*.*
	rm -f kernel/build/*.*
	rm -f bootloader/bootloader.bin
	rm -f kernel/kernel.bin

bootloader/bootloader.bin: bootloader/src/** bootloader/include/**
	make -C bootloader

kernel/kernel.bin: kernel/src/** kernel/include/**
	make -C kernel

# build image preserving file system
$(OUTPUT): bootloader/bootloader.bin kernel/kernel.bin
	nasm -f bin -o os_temp.bin fireflyos.asm
	mv os_temp.bin $(OUTPUT)
	sudo mount -o loop,offset=1048576 fireflyos.bin fireflyfs
	sudo mkdir -p fireflyfs/boot/modules
	sudo cp kernel/kernel.bin fireflyfs/boot/kernel.bin
	sudo umount fireflyfs

# restart with empty filesystem
$(OUTPUT)_new: clean.bin
	cp clean.bin fireflyos.bin
