project("FireflyOS")

cmake_minimum_required(VERSION 3.16.0)


set(CXX_COMPILER_FLAGS "")
set(CMAKE_CXX_COMPILER /usr/bin/clang++)

if (NOT DEFINED ARCH) # Default arch
    set(ARCH i386)
endif()

include(include/stl/CMakeLists.txt)
include(arch/${ARCH}/CMakeLists.txt)

add_custom_command(TARGET firefly_kernel_${ARCH}
                   PRE_LINK 
                   COMMAND make build ARCH=${ARCH} CSTDLIB=)

