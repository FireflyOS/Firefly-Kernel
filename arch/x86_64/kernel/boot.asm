global start
extern _kernel_end
extern _init_array_end
extern kernel_main

VIRT_ADDR equ 0xFFFFFFFF80000000

section .rodata
%include "arch/x86_64/kernel/multiboot2.asm"

gdt64:
    dq 0                                        ; zero entry
.code: equ $ - gdt64                            ; code segment
                                                ; executable | code | present | x86_64 code
    dq (1<<43) | (1<<44) | (1<<47) | (1<<53) 
.pointer:
    dw $ - gdt64 - 1
    dq gdt64 - VIRT_ADDR                        ; point to physical address of gdt

section .bss                                    ; reserve space for the paging structures
align 4096
pml4:
    resb 4096
pml3:
    resb 4096
pml2:
    resb 4096
pml1:
    resb 4096
stack_bottom:
    resb 65536
stack_top:

section .multiboot_header
header_start:
    dd 0xe85250d6                               ; magic number (multiboot 2)
    dd 0                                        ; architecture 0 (protected mode i386)
    dd header_end - header_start                ; header length
                                                ; checksum
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))
header_end:
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
    dw 0    ; type
    dw 0    ; flags
    dd 8    ; size
tag_end_end:

section .pm_stub
bits 32
magic: dd 0x0
multiboot2_struct: dd 0x0

start:
    mov [magic], eax             ; Preserve multiboot2 structure
    mov [multiboot2_struct], ebx ; Preserve multiboot2 magic

    mov esp, stack_top - VIRT_ADDR

    call check_multiboot
    call check_cpuid
    call check_long_mode

    call set_up_page_tables

                                                ; enable long mode and paging

    mov eax, cr4
    or eax, 0b110000                            ; set PAE and PSE
    mov cr4, eax

    mov eax, pml4 - VIRT_ADDR                   ; point cr3 to pml4
    mov cr3, eax

    mov ecx, 0xC0000080                         ; get EFER MSR
    rdmsr
    or eax, 1 << 8                              ; set LME
    wrmsr

    mov eax, cr0
    or eax, 1 << 31                             ; set PG
    mov cr0, eax

    lgdt [(gdt64.pointer - VIRT_ADDR)]
    jmp gdt64.code:.stub64

bits 64
.stub64:                                        ; trampoline to higher half
    mov rsp, stack_top
    mov rax, qword long_mode_start
    jmp rax

bits 32
error:                                          ; Prints `ERR: ` and the given error code to screen and hangs.
                                                ; parameter: error code (in ascii) in al
    mov dword [0xb8000], 0x4f524f45
    mov dword [0xb8004], 0x4f3a4f52
    mov dword [0xb8008], 0x4f204f20
    mov byte  [0xb800a], al
    hlt

check_multiboot:
    cmp eax, 0x36d76289
    jne .no_multiboot 
    ret
.no_multiboot:
    mov al, "0"
    jmp error 

check_cpuid:                                    ; Check if CPUID is supported by attempting to flip the ID bit
                                                ; in the flags register. If we can flip it, CPUID is available.

    pushfd                                      ; copy flags in to eax
    pop eax

    mov ecx, eax                                ; copy original to ecx

    xor eax, 1 << 21                            ; flip the ID bit

    push eax                                    ; copy into flags
    popfd

    pushfd                                      ; put back into eax
    pop eax

    push ecx                                    ; put original back into flags
    popfd

    cmp eax, ecx                                ; modified flags same as original?
    je .no_cpuid                                ; yes, cpuid not available
    ret

.no_cpuid:
    mov al, "1"
    jmp error

check_long_mode:
    mov eax, 0x80000000                         ; get max extended input value for cpuid
    cpuid
    cmp eax, 0x80000001                         ; support at least one extended function?
    jb .no_long_mode                            ; no, can't check for long mode

    mov eax, 0x80000001                         ; get extended processor info
    cpuid
    test edx, 1 << 29                           ; LM bit set?
    jz .no_long_mode                            ; no, no long mode
    ret

.no_long_mode:
    mov al, "2"
    jmp error

set_up_page_tables:
                                                ; first map to lower virtual address
    mov eax, pml3 - VIRT_ADDR
    or eax, 0b11
    mov [pml4 - VIRT_ADDR], eax

    mov eax, pml2 - VIRT_ADDR
    or eax, 0b11
    mov [pml3 - VIRT_ADDR], eax

    mov ecx, 0                                  ; count entries

.map_pd:
                                                ; map ecx-th page directory entry to a 2MiB page
                                                ; that starts at address 2MiB*ecx
    mov eax, 0x200000                           ; 2MiB
    mul ecx                                     ; start address of ecx-th page
    or eax, 0b10000011                          ; present + writable + page size
    mov [(pml2 - VIRT_ADDR) + ecx * 8], eax       ; map ecx-th entry

    inc ecx                                     ; increase counter
    cmp ecx, 512                                ; if counter == 512, the whole page directory table is mapped
    jne .map_pd                                 ; else map the next entry

                                                ; now set up higher virtual address

    mov eax, pml3 - VIRT_ADDR                   ; physical address of a pml3
    or eax, 0b11                                ; writable + present
    mov dword [(pml4 - VIRT_ADDR) + 4088], eax  ; put in last entry of pml4, so
                                                ; bits 63:39 of virtual address all 1's

    mov eax, pml2 - VIRT_ADDR                     ; physical address of a pml2
    or eax, 0b11                                ; writable + present
    mov dword [(pml3 - VIRT_ADDR) + 4080], eax  ; put in second-to-last entry of pml3 so 
                                                ; bits 38:31 are all 1's, and bit 30 is 0,
                                                ; 0xFFFF_FFFF_8nnn_nnnn

    xor eax, eax                                ; begin mapping physical address 0
    xor ebx, ebx                                ; page table entry, start at index 0

.map_pt:                                        ; now map the page table to
                                                ; the physical addresses up to the end of the kernel
    cmp eax, _kernel_end - VIRT_ADDR            ; passed kernel binary?
    je .done                                    ; yes, done mapping
    or eax, 0b11                                ; writable + present
    mov dword [(pml1 - VIRT_ADDR) + ebx], eax     ; put into pml1
    add ebx, 8                                  ; next index
    and eax, ~0b11                              ; clear bottom two bits
    add eax, 4096                               ; next physical 4KiB frame
    jmp .map_pt                                 ; map next entry

.done:
    ret

section .text
bits 64
long_mode_start:

                                                ; now make pml2's first entry point to a pml1
                                                ; since earlier it mapped a 2MiB page
    mov eax, pml1 - VIRT_ADDR                     ; physical address of a pml1
    or eax, 0b11                                ; writable + present
    mov qword [(pml2 - VIRT_ADDR)], rax           ; put in first entry

    mov ax, 0
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

                                                ; call global constructors
    lea rbx, [rel _init_array_end-8]
                                                ; Since all of our code and data is within +/-2GiB
                                                ; we can use a RIP relative instruction with disp32

    cld

    ; Note: The use of edi/esi over rdi/rsi is due to grub storing a
    ; 32 bit value in eax/ebx, so passing a 32 bit register to kernel_main
    ; makes our life a lot easier.
    mov edi, [magic]
    mov esi, [multiboot2_struct]
    jmp kernel_main


early_map:
.setup:
    ; early_map(void* from, void* to, int flags)
    ; rsi = from
    ; rdi = to
    ; rdx = flags
.map:
    cmp eax, [rsi]; - VIRT_ADDR            ; Done mapping?
    je .done                                    ; Done mapping
    or eax, [rdx]                                 ; Set flags
    mov dword [(pml1 - VIRT_ADDR) + ebx], eax   ; put into pml1
    add ebx, 8                                  ; next index
    and eax, ~0b11                              ; clear bottom two bits
    add eax, 4096                               ; next physical 4KiB frame
    jmp .map                                    ; map next entry
.done:
    ret