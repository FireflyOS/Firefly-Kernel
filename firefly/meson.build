kernel_build_flags = [
    '-target', 'x86_64-unknown-elf', '-m64', '-mcmodel=kernel',
    '-std=c++20', '-Werror', '-Wall', '-Wextra', '-pedantic',
    '-g', '-nostdlib', '-fno-builtin', '-fPIC',
    '-mno-red-zone', '-fno-stack-check', '-fno-stack-protector',
    '-fno-omit-frame-pointer', '-ffreestanding', '-fno-exceptions',
    '-fno-rtti', '-Wno-zero-length-array', '-mno-80387', '-mno-mmx',
    '-mno-3dnow', '-mno-sse', '-mno-sse2', '-Wno-c99-extensions', '-Wno-gnu'
]

cxx_files += files('libk++/bitmap.cpp', 'libk++/fmt.cpp', 'libk++/utils.cpp')
cxx_files += files(
    'kernel/kernel.cpp', 'kernel/stubs.cpp', 'kernel/drivers/ps2.cpp',
    'kernel/drivers/serial.cpp', 'kernel/intel64/int/interrupt.cpp', 'kernel/memory-manager/primary/primary_phys.cpp',
    'kernel/intel64/gdt/gdt.cpp', 'kernel/intel64/gdt/tss.cpp', 'kernel/init/init.cpp',
    'kernel/trace/strace.cpp', 'kernel/trace/symbols.cpp', 'kernel/memory-manager/virtual/virtual.cpp',
    'kernel/console/stivale2-term.cpp', 'kernel/intel64/paging.cpp'
)
asm_files += files('kernel/intel64/gdt/gdt.asm', 'kernel/intel64/int/interrupt.asm')