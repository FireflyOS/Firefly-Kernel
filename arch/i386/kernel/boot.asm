bits 32

global start
extern kernel_main

section .rodata
%include "arch/i386/kernel/multiboot2.asm"

section .multiboot_header
header_start:
    dd 0xe85250d6                               ; magic number (multiboot 2)
    dd 0                                        ; architecture 0 (protected mode i386)
    dd header_end - header_start                ; header length
                                                ; checksum
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))

                                                ; insert optional multiboot tags here

                                                ; required end tag
    dw 0    ; type
    dw 0    ; flags
    dd 8    ; size
framebuffer_tag_start:  
        dw MULTIBOOT_HEADER_TAG_FRAMEBUFFER
        dw MULTIBOOT_HEADER_TAG_OPTIONAL
        dd framebuffer_tag_end - framebuffer_tag_start
        dd 1024
        dd 768
        dd 32
framebuffer_tag_end:
header_end:

section .pm_stub
bits 32
start:
    push 0
    popf
    
    push ebx
    push eax
	call kernel_main
	jmp $