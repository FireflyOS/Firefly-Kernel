global start
extern _kernel_end
extern _kernel_start
extern kernel_main
extern init_array_end

VIRT_ADDR equ 0xFFFFFFFF80100000

section .multiboot_header
header_start:
    dd 0xe85250d6                           ; magic number (multiboot 2)
    dd 0                                    ; architecture 0 (protected mode i386)
    dd header_end - header_start            ; header length
                                            ; checksum
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))

                                            ; insert optional multiboot tags here

                                            ; required end tag
    dw 0    ; type
    dw 0    ; flags
    dd 8    ; size
header_end:

section .bss                                ; reserve space for the paging structures
align 4096
pml4:
    resb 4096
pdpt:
    resb 4096
pd:
    resb 4096
pt:
    resb 4096
stack_bottom:
    resb 65536
stack_top:

section .pm_stub
bits 32
start:
    mov esp, stack_top - VIRT_ADDR

    call check_multiboot
    call check_cpuid
    call check_long_mode

    call enable_sse
    call set_up_page_tables

    mov eax, cr4                            ; enable long mode and paging
    or eax, 1 << 5                          ; set PAE
    mov cr4, eax

    mov eax, pml4 - VIRT_ADDR               ; point cr3 to pml4
    mov cr3, eax

    mov ecx, 0xC0000080                     ; get EFER MSR
    rdmsr
    or eax, 1 << 8                          ; set LME
    wrmsr

    mov eax, cr0
    or eax, 1 << 31                         ; set PG
    mov cr0, eax

    lgdt [(gdt64.pointer - VIRT_ADDR)]
    jmp (gdt64.code - VIRT_ADDR):.stub64    ; warning: word data exceeds bounds

bits 64
.stub64:                                    ; trampoline to higher half
    mov rsp, stack_top
    mov rax, qword long_mode_start
    jmp rax

bits 32
error:                                      ; Prints `ERR: ` and the given error code to screen and hangs.
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

check_cpuid:                                ; cCheck if CPUID is supported by attempting to flip the ID bit
                                            ; in the flags register. If we can flip it, CPUID is available.

    pushfd                                  ; copy flags in to eax
    pop eax

    mov ecx, eax                            ; copy original to ecx

    xor eax, 1 << 21                        ; flip the ID bit

    push eax                                ; copy into flags
    popfd

    pushfd                                  ; put back into eax
    pop eax

    push ecx                                ; put original back into flags
    popfd

    cmp eax, ecx                            ; modified flags same as original?
    je .no_cpuid                            ; yes, cpuid not available
    ret

.no_cpuid:
    mov al, "1"
    jmp error

check_long_mode:
    mov eax, 0x80000000                     ; get max extended input value for cpuid
    cpuid
    cmp eax, 0x80000001                     ; support at least one extended function?
    jb .no_long_mode                        ; no, can't check for long mode

    mov eax, 0x80000001                     ; get extended processor info
    cpuid
    test edx, 1 << 29                       ; LM bit set?
    jz .no_long_mode                        ; no, no long mode
    ret

.no_long_mode:
    mov al, "2"
    jmp error

enable_sse:
    mov eax, cr0
    and ax, 0xFFFB                          ; clear coprocessor emulation CR0.EM
    or ax, 0x2                              ; set coprocessor monitoring  CR0.MP
    mov cr0, eax
    mov eax, cr4
    or ax, 3 << 9                           ; set CR4.OSFXSR and CR4.OSXMMEXCPT at the same time
    mov cr4, eax
    ret

set_up_page_tables:
    mov eax, pdpt - VIRT_ADDR                   ; physical address of a pdpt
    or eax, 0b11                                ; writable + present
    mov dword [(pml4 - VIRT_ADDR) + 4058], eax  ; put in last entry of pml4, so
                                                ; bits 63:39 of virtual address all 1's

    mov eax, pd - VIRT_ADDR                     ; physical address of a pd
    or eax, 0b11                                ; writable + present
    mov dword [(pdpt - VIRT_ADDR) + 4050], eax  ; put in second-to-last entry of pdpt so 
                                                ; bits 38:31 are all 1's, and bit 30 is 0,
                                                ; 0xFFFF_FFFF_8nnn_nnnn

    mov eax, pt - VIRT_ADDR                 ; physical address of a pt
    or eax, 0b11                            ; writable + present
    mov dword [(pd - VIRT_ADDR)], eax                     ; put in first entry
                                            ; bits 29:21 all 0's

                                            ; one pt can map 2MiB of memory, so linker.ld
                                            ; asserts that the kernel is smaller than 2MiB
                                            ; so we can map it all with one pt,
                                            ; if the assertion fails we need a second pt

    xor eax, eax                            ; begin mapping physical address 0
    xor ebx, ebx                            ; page table entry, start at index 0

.map_pt:                                    ; now map the page table to
                                            ; the physical addresses up to the end of the kernel

    cmp eax, _kernel_end - VIRT_ADDR        ; passed kernel binary?
    je .done                                ; yes, done mapping
    or eax, 0b11                            ; writable + present
    mov dword [(pt - VIRT_ADDR) + ebx], eax               ; put into pt
    add ebx, 8                              ; next index
    and eax, ~0b11                          ; clear bottom two bits
    add eax, 4096                           ; next physical 4KiB frame
    jmp .map_pt                             ; map next entry

.done:
    ret

section .rodata
gdt64:
    dq 0                                    ; zero entry
.code: equ $ - gdt64                        ; code segment
                                            ; executable | code | present | x86_64 code
    dq (1<<43) | (1<<44) | (1<<47) | (1<<53) 
.pointer:
    dw $ - gdt64 - 1
    dq gdt64

section .text
bits 64
long_mode_start:
    mov ax, 0
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

                                            ; call global constructors
    lea rbx, [rel init_array_end-8]
                                            ; Since all of our code and data is within +/-2GiB
                                            ; we can use a RIP relative instruction with disp32
    jmp .getaddr
.docall:
    call r12
.getaddr:
    mov r12, [rbx]
    sub rbx, 8
    test r12, r12
;    cmp r12, -1                            ; Should we be testing for 0 or -1?
    jnz .docall

    cld

    jmp kernel_main