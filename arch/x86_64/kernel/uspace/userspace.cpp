#include "x86_64/uspace/userspace.hpp"
#include <stdint.h>

namespace firefly::kernel
{
    UserSpace::UserSpace()
    {
        // Enable sysret & syscall
        asm volatile(
            ".intel_syntax noprefix\n"
            "mov rcx, 0xc0000082\n"
            "wrmsr\n"
            "mov rcx, 0xc0000080\n"
            "rdmsr\n"
            "or eax, 1\n"
            "wrmsr\n"
            "mov rcx, 0xc0000081\n"
            "rdmsr\n"
            "mov edx, 0x00180008\n"
            "wrmsr\n"
            ".att_syntax prefix\n"
        );
    }

    void UserSpace::jump(void (*fn)())
    {
        asm volatile(
            ".intel_syntax noprefix\n"
            "mov ax, (4 * 8) | 3\n" //Specify DS segement for DPL 3
            "mov ds, ax\n"
            "mov es, ax\n" 
            "mov fs, ax\n" 
            "mov gs, ax\n"
            "mov r11, 0x202\n" // EFLAGS
            "sysretq\n"
            ".att_syntax prefix\n"
            :: "c"((uintptr_t)fn)
        );
    }

} // namespace firefly::kernel
