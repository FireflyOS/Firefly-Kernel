kernel_build_flags = [
	'-std=c++20',
    '-ffreestanding',
	'-g', '-O2',
    '-I include/',
	'-I include/cxxshim',
	'-I include/frigg',
    '-ffreestanding',
    '-fno-stack-protector',
    '-fno-stack-check',
    '-fno-pie',
    '-fno-pic',
    '-m64',
    '-mno-80387',
    '-mno-mmx',
    '-mno-sse',
    '-mno-sse2',
    '-mno-red-zone',
    '-mcmodel=kernel',
    '-fno-exceptions',
    '-fno-rtti',
    '-mno-3dnow'
]

cxx_files += files('libk++/fmt.cpp', 'libk++/memory.cpp', 'libk++/utils.cpp')
cxx_files += files(
    'kernel/kernel.cpp', 'kernel/stubs.cpp', 'kernel/drivers/ps2.cpp',
    'kernel/drivers/serial.cpp', 'kernel/intel64/int/interrupt.cpp', 'kernel/memory-manager/primary/primary_phys.cpp',
    'kernel/intel64/gdt/gdt.cpp', 'kernel/intel64/gdt/tss.cpp', 'kernel/init/init.cpp',
    'kernel/trace/strace.cpp', 'kernel/trace/symbols.cpp', 'kernel/memory-manager/virtual/virtual.cpp',
    'kernel/console/console.cpp', 'kernel/console/limine-terminal-port/gterm.c', 'kernel/console/limine-terminal-port/image.c',
	'kernel/console/limine-terminal-port/term.c', 'kernel/console/limine-terminal-port/tterm.c',
	'kernel/intel64/paging.cpp', 'kernel/intel64/cpu/cpu.cpp',
)
asm_files += files('kernel/intel64/gdt/gdt.asm', 'kernel/intel64/int/interrupt.asm')