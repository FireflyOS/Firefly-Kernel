bits 32

global start
extern kernel_main

section .rodata
; Magic
%define MULTIBOOT2_HEADER_MAGIC 0xe85250d6

; Tags
%define GRUB_MULTIBOOT_ARCHITECTURE_I386 0
%define MULTIBOOT_HEADER_TAG_END  0
%define MULTIBOOT_HEADER_TAG_OPTIONAL 1
%define MULTIBOOT_HEADER_TAG_FRAMEBUFFER 5
%define MULTIBOOT_HEADER_TAG_OPTIONAL 1
%define MULTIBOOT_HEADER_TAG_ADDRESS 2
%define MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS 3

section .multiboot_header
; align 8
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

section .bss
stack_bottom:
    resb 65536
stack_top:

section .pm_stub
start:
    mov esp, stack_top

    push 0
    popf
    
    push ebx
    push eax
	call kernel_main
	jmp $