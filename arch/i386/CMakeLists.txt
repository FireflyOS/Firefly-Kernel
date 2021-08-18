set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a -DI386 -ggdb -target i386-unknown-elf -m32 -mcmodel=kernel -Wall -Wextra -pedantic -Werror -g -nostdlib -fno-builtin -fno-PIC -mno-red-zone -fno-stack-check -fno-stack-protector -fno-omit-frame-pointer -ffreestanding -fno-exceptions -fno-rtti -Wno-zero-length-array -mno-80387 -mno-mmx -mno-3dnow -mno-sse -mno-sse2 -Wno-gnu")

add_library(firefly_libk_i386
    ./libk++/iostream.cpp
)

add_library(firefly_kernel_i386
    ./kernel/drivers/serial.cpp
    ./kernel/drivers/vbe.cpp
    ./kernel/drivers/vga.cpp

    ./kernel/gdt/gdt.cpp

    ./kernel/init/init.cpp
    ./kernel/init/mb2_proto.cpp

    ./kernel/int/interrupt.cpp
    ./kernel/int/pic.cpp

    ./kernel/memory-manager/bootstrap-buddy.cpp

    ./kernel/pmm/buddy.cpp

    ./kernel/trace/strace.cpp

    ./kernel/kernel.cpp
)

target_include_directories(firefly_libk_i386
    PUBLIC ./include    
    PUBLIC ./include/i386
)

target_link_libraries(firefly_libk_i386
    PUBLIC firefly_stdlib
)

target_link_libraries(firefly_kernel_i386
    PRIVATE firefly_stdlib
    PRIVATE firefly_libk_i386
)
