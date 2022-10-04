kernel_cxx_flags = [
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
    '-mno-3dnow',
    '-fno-omit-frame-pointer'
]

kernel_c_flags = [
    '-std=gnu11',
    '-ffreestanding', '-g',
    '-I include',
    '-ffreestanding',
    '-fno-stack-protector',
    '-fno-stack-check',
    '-fno-pie',
    '-fno-pic',
    '-m64',
    '-march=x86-64',
    '-mabi=sysv',
    '-mno-80387',
    '-mno-mmx',
    '-mno-sse',
    '-mno-sse2',
    '-mno-red-zone',
    '-mcmodel=kernel',
    '-fno-omit-frame-pointer'
]

c_files += files(
    'kernel/console/gterm.c', 'kernel/console/image.c',
    'kernel/console/term.c'
)

cxx_files += files('libk++/fmt.cpp', 'libk++/memory.cpp', 'libk++/utils.cpp', 'libk++/cstring.cpp')
cxx_files += files(
    'kernel/kernel.cpp', 'kernel/stubs.cpp', 'kernel/drivers/ps2.cpp', 'kernel/logger.cpp',
    'kernel/drivers/serial.cpp', 'kernel/intel64/int/interrupt.cpp', 'kernel/memory-manager/primary/primary_phys.cpp',
    'kernel/intel64/gdt/gdt.cpp', 'kernel/intel64/gdt/tss.cpp', 'kernel/init/init.cpp',
    'kernel/trace/strace.cpp', 'kernel/trace/symbols.cpp', 'kernel/memory-manager/virtual/virtual.cpp',
    'kernel/console/console.cpp', 'kernel/intel64/acpi/acpi.cpp', 'kernel/graphics/framebuffer.cpp', 
    'kernel/intel64/paging.cpp', 'kernel/intel64/cpu/cpu.cpp', 'kernel/intel64/cpu/ap/ap.cpp',
    'kernel/intel64/cpu/apic/apic.cpp', 'kernel/intel64/pit/pit.cpp',
    'kernel/intel64/cpu/apic/ioapic.cpp', 'kernel/intel64/cpu/cpu.cpp', 'kernel/trace/sanitizer/kasan.cpp',
    'kernel/memory-manager/secondary/heap.cpp',
    'kernel/timer/timer.cpp', 'kernel/timer/hpet.cpp'
)
asm_files += files('kernel/intel64/gdt/gdt.asm', 'kernel/intel64/int/interrupt.asm')
