project("FireflyOS")

cmake_minimum_required(VERSION 3.16.0)

include(include/stl/CMakeLists.txt)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


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

