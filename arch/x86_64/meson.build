kernel_build_flags = [
    '-target', 'x86_64-unknown-elf', '-m64', '-mcmodel=kernel',
    '-std=c++20', '-Werror', '-Wall', '-Wextra', '-pedantic',
    '-g', '-nostdlib', '-fno-builtin', '-fPIC',
    '-mno-red-zone', '-fno-stack-check', '-fno-stack-protector',
    '-fno-omit-frame-pointer', '-ffreestanding', '-fno-exceptions',
    '-fno-rtti', '-Wno-zero-length-array', '-mno-80387', '-mno-mmx',
    '-mno-3dnow', '-mno-sse', '-mno-sse2', '-Wno-c99-extensions', '-Wno-gnu'
]

cxx_files += files('libk++/bitmap.cpp', 'libk++/iostream.cpp')
cxx_files += files(
    'kernel/kernel.cpp', 'kernel/utils.cpp', 'kernel/drivers/ps2.cpp',
    'kernel/drivers/serial.cpp', 'kernel/drivers/vbe.cpp', 'kernel/int/interrupt.cpp',
    'kernel/fb/framebuffer.cpp', 'kernel/gdt/gdt.cpp', 'kernel/gdt/tss.cpp', 'kernel/init/init.cpp',
    'kernel/memory-manager/secondary/buddy/buddy_heap.cpp', 'kernel/memory-manager/secondary/buddy/buddy.cpp',
    'kernel/scheduler/spinlock.cpp', 'kernel/trace/strace.cpp', 'kernel/trace/symbols.cpp',
    'kernel/uspace/userspace.cpp', 'kernel/memory-manager/virtual/virtual.cpp',
    'kernel/fb/stivale2-term.cpp'
)
asm_files += files('kernel/gdt/gdt.asm', 'kernel/int/interrupt.asm')