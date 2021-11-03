cmake_minimum_required(VERSION 3.16.3)

set(CMAKE_CXX_COMPILER "clang++")

project(FireflyOS CXX ASM_NASM)

enable_language(ASM_NASM)
set(CMAKE_SYSTEM_PROCESSOR "x86_64")
set(CMAKE_CXX_COMPILER_TARGET "x86_64-elf")
set(CMAKE_CXX_FLAGS "-target x86_64-unknown- -m64 -mcmodel=kernel -g -nostdlib -fno-builtin -fno-PIC -fpie -mno-red-zone -fno-stack-check -fno-stack-protector -fno-omit-frame-pointer -ffreestanding -fno-exceptions -fno-rtti -Wno-zero-length-array -mno-80387 -mno-mmx -mno-3dnow -mno-sse -mno-sse2 -Wno-c99-extensions -Wno-gnu")
set(LINKER_FLAGS "--no-undefined -T ../linkage/linker_x86_64.ld -nostdlib -m elf_x86_64")

# There's probably a better way to do this but oh well.
set(CMAKE_ASM_NASM_COMPILE_OBJECT "nasm -f elf64 ../arch/x86_64/kernel/gdt/gdt.asm -o CMakeFiles/kernel_x86_64.elf.dir/arch/x86_64/kernel/gdt/gdt.asm.o")
set(CMAKE_ASM_NASM_COMPILE_OBJECT "nasm -f elf64 ../arch/x86_64/kernel/int/interrupt.asm -o CMakeFiles/kernel_x86_64.elf.dir/arch/x86_64/kernel/int/interrupt.asm.o")

file(GLOB_RECURSE SRC_FILES "arch/*.cpp" "arch/*.asm")
file(GLOB_RECURSE OBJ_FILES "*.o")
set(CMAKE_CXX_LINK_EXECUTABLE "ld.lld ${LINKER_FLAGS} ${OBJ_FILES}")

add_subdirectory("include/stl")

include_directories("include")
include_directories("include/stl")

add_executable(kernel_x86_64.elf ${SRC_FILES})
target_link_libraries(kernel_x86_64.elf PUBLIC STL)
set_property(TARGET kernel_x86_64.elf PROPERTY CXX_STANDARD 20)