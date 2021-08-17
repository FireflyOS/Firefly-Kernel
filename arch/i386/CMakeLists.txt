
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a -DI386 -ggdb -target i386-unknown-elf -m32 -mcmodel=kernel -Wall -Wextra -pedantic -Werror -g -nostdlib -fno-builtin -fno-PIC -mno-red-zone -fno-stack-check -fno-stack-protector -fno-omit-frame-pointer -ffreestanding -fno-exceptions -fno-rtti -Wno-zero-length-array -mno-80387 -mno-mmx -mno-3dnow -mno-sse -mno-sse2 -Wno-gnu")

add_library(firefly_libk_i386
    ./arch/i386/libk++/iostream.cpp
)

add_executable(firefly_kernel_i386
    ./arch/i386/kernel/drivers/serial.cpp
    ./arch/i386/kernel/drivers/vbe.cpp
    ./arch/i386/kernel/drivers/vga.cpp
    
    ./arch/i386/kernel/gdt/gdt.cpp
    
    ./arch/i386/kernel/init/init.cpp
    ./arch/i386/kernel/init/mb2_proto.cpp
    
    ./arch/i386/kernel/int/interrupt.cpp
    ./arch/i386/kernel/int/pic.cpp
    
    ./arch/i386/kernel/memory-manager/bootstrap-buddy.cpp
    
    ./arch/i386/kernel/pmm/buddy.cpp
    
    ./arch/i386/kernel/trace/strace.cpp
    
    ./arch/i386/kernel/kernel.cpp
    )

target_include_directories(firefly_libk_i386
    PUBLIC ./include    
    PUBLIC ./include/i386
)

# target_include_directories(firefly_kernel_i386
#     PRIVATE ./include    
#     PRIVATE ./include/i386
# )

target_link_libraries(firefly_kernel_i386
    PRIVATE firefly_stdlib
    PRIVATE firefly_libk_i386
)

