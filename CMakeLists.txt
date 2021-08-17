project("FireflyOS")

cmake_minimum_required(VERSION 3.16.0)

set(CMAKE_C_COMPILER /usr/bin/clang)
set(CMAKE_CXX_COMPILER /usr/bin/clang++)
set(CMAKE_ASM_COMPILER /usr/bin/nasm)

enable_language(ASM_NASM)
set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <INCLUDES> <FLAGS> -o <OBJECT> <SOURCE>")

# Create a compile option that operates on ASM_NASM files
# If the target has a property NASM_OBJ_FORMAT, use it, otherwise
# use the environment variable CMAKE_ASM_NASM_OBJECT_FORMAT
add_compile_options(
    "$<$<COMPILE_LANGUAGE:ASM_NASM>:-f $<IF:$<BOOL:$<TARGET_PROPERTY:NASM_OBJ_FORMAT>>, \
    $<TARGET_PROPERTY:NASM_OBJ_FORMAT>, ${CMAKE_ASM_NASM_OBJECT_FORMAT}>>"
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_FLAGS "-ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -DI386")
set(CMAKE_C_FLAGS "-ffreestanding -O2 -Wall -Wextra")

set(KERNEL_BIN "FireflyOS.bin")
set(KERNEL_ISO ${CMAKE_PROJECT_NAME}.iso)

## A simple lib to test C compilation and linking
# add_library(mylib mylib.c)
add_subdirectory(include/stl)
add_subdirectory(arch/i386)

## Building the kernel binary
add_executable(${KERNEL_BIN} ./arch/i386/kernel/boot.asm ./arch/i386/kernel/kernel.cpp)
target_include_directories(${KERNEL_BIN} PRIVATE ./include)
target_link_libraries(${KERNEL_BIN} PRIVATE firefly_stdlib firefly_kernel_i386)
set_target_properties(${KERNEL_BIN} PROPERTIES 
    LINK_FLAGS "-T ${CMAKE_SOURCE_DIR}/linkage/linkeri386.ld -ffreestanding -O2 -nostdlib -lgcc" 
    NASM_OBJ_FORMAT elf64
)
add_custom_command(TARGET ${KERNEL_BIN} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/bin
        COMMAND ${CMAKE_COMMAND} -E copy ${KERNEL_BIN} ${CMAKE_SOURCE_DIR}/bin/)

## Generating the ISO file
add_custom_target(${KERNEL_ISO}
        COMMAND ../generate-iso.sh
        COMMENT "Generating the kernel bootable iso file"
        BYPRODUCTS ${KERNEL_ISO})
add_dependencies(FireflyOS.bin FireflyOS.bin)
add_custom_command(TARGET ${KERNEL_ISO} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/bin
        COMMAND ${CMAKE_COMMAND} -E copy ${KERNEL_ISO} ${CMAKE_SOURCE_DIR}/bin/)