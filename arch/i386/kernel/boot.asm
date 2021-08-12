bits 32

global start
extern kernel_main

section .rodata
%include "arch/i386/kernel/multiboot2.asm"

section .multiboot_header
header_start:
    dd MULTIBOOT2_HEADER_MAGIC                  ; magic number (multiboot 2)
    dd 0                                        ; architecture 0 (protected mode i386)
    dd header_end - header_start                ; header length
                                                ; checksum
    dd 0x100000000 - (MULTIBOOT2_HEADER_MAGIC + (header_end - header_start))

framebuffer_tag_start:  
    dw MULTIBOOT_HEADER_TAG_FRAMEBUFFER
    dw MULTIBOOT_HEADER_TAG_OPTIONAL
    dd framebuffer_tag_end - framebuffer_tag_start
    dd 0  ; Width - no preference
    dd 0  ; Height - no preference
    dd 0  ; bpp - no preference
framebuffer_tag_end:

align 8
tag_end:
    dw MULTIBOOT_HEADER_TAG_END
    dw 0
    dd 8
tag_end_end:
header_end:

section .pm_stub
start:
    push 0
    popf
    
    push ebx
    push eax
	call kernel_main
	jmp $