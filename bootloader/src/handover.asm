VIRT_ADDR equ 0xFFFFFFFF80000000

bits 16

global handover

section .text

handover:
    mov bp, sp
    mov edi, dword [bp+4]
    lgdt [gdt32.pointer]
    mov eax, cr0
    or al, 1
    mov cr0, eax
    jmp gdt32.code:.stub32

bits 32
.stub32:

    mov esp, stack_top

    call check_cpuid
    call check_long_mode
    ;call enable_sse
    call set_up_page_tables
                                                ; enable long mode and paging
    mov eax, cr4
    or eax, 0b110000                            ; set PAE and PSE
    mov cr4, eax

    mov eax, pml4                               ; point cr3 to pml4
    mov cr3, eax

    mov ecx, 0xC0000080                         ; get EFER MSR
    rdmsr
    or eax, 1 << 8                              ; set LME
    wrmsr

    mov eax, cr0
    or eax, 0x80000000                             ; set PG
    mov cr0, eax
.lab:
    lgdt [gdt64.pointer]
    jmp gdt64.code:.stub64

bits 64
.stub64:                                        ; trampoline to higher half
    mov rsp, stack_top
    jmp long_mode_start

bits 32

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
    mov al, '1'
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
    mov al, '2'
    jmp error

enable_sse:
    mov eax, cr0
    and ax, 0xFFFB                              ; clear coprocessor emulation CR0.EM
    or ax, 0x2                                  ; set coprocessor monitoring  CR0.MP
    mov cr0, eax
    mov eax, cr4
    or ax, 3 << 9                               ; set CR4.OSFXSR and CR4.OSXMMEXCPT at the same time
    mov cr4, eax
    ret

set_up_page_tables:
                                                ; first map to lower virtual address
    mov eax, pdpt
    or eax, 0b11
    mov [pml4], eax

    mov eax, pd
    or eax, 0b11
    mov [pdpt], eax

    mov ecx, 0                                  ; count entries

.map_pd:
                                                ; map ecx-th page directory entry to a 2MiB page
                                                ; that starts at address 2MiB*ecx
    mov eax, 0x200000                           ; 2MiB
    mul ecx                                     ; start address of ecx-th page
    or eax, 0b10000011                          ; present + writable + page size
    mov [pd + ecx * 8], eax                      ; map ecx-th entry

    inc ecx                                     ; increase counter
    cmp ecx, 512                                ; if counter == 512, the whole page directory table is mapped
    jne .map_pd                                 ; else map the next entry

                                                ; now set up higher virtual address

    mov eax, pdpt                               ; physical address of a pdpt
    or eax, 0b11                                ; writable + present
    mov dword [pml4 + 4088], eax                ; put in last entry of pml4, so
                                                ; bits 63:39 of virtual address all 1's

    mov eax, pd                                 ; physical address of a pd
    or eax, 0b11                                ; writable + present
    mov dword [pdpt + 4080], eax                ; put in second-to-last entry of pdpt so 
                                                ; bits 38:31 are all 1's, and bit 30 is 0,
                                                ; 0xFFFF_FFFF_8nnn_nnnn

    xor eax, eax                                ; begin mapping physical address 0
    xor ebx, ebx                                ; page table entry, start at index 0

.map_pt:                                        ; now map the page table to
                                                ; the physical addresses up to the end of the kernel

    cmp eax, 0x00102000           ; passed kernel binary?
    je .done                                    ; yes, done mapping
    or eax, 0b11                                ; writable + present
    mov dword [pt + ebx], eax     ; put into pt
    add ebx, 8                                  ; next index
    and eax, ~0b11                              ; clear bottom two bits
    add eax, 4096                               ; next physical 4KiB frame
    jmp .map_pt                                 ; map next entry

.done:
    ret

error:                                          ; Prints `ERR: ` and the given error code to screen and hangs.
                                                ; parameter: error code (in ascii) in al
    mov dword [0xb8000], 0x4f524f45
    mov dword [0xb8004], 0x4f3a4f52
    mov dword [0xb8008], 0x4f204f20
    mov byte  [0xb800a], al
.lop:
    hlt
    jmp .lop

bits 64
long_mode_start:
                                                ; now make pd's first entry point to a pt
                                                ; since earlier it mapped a 2MiB page
    ;mov eax, pt                         ; physical address of a pt
    ;or eax, 0b11                                ; writable + present
    ;mov dword [pd], eax                 ; put in first entry

    mov ax, 0
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    and rdi, 0xffffffff

    jmp 0x00100000 + VIRT_ADDR

section .bss                                    ; reserve space for the paging structures
align 4096
pml4:
    resb 4096
pdpt:
    resb 4096
pd:
    resb 4096
pt:
    resb 4096
    resb 4096
stack_top:

section .rodata
gdt32:
    dq 0
.code: equ $ - gdt32
    dq 0x00cf9a000000ffff
.data: equ $ - gdt32
    dq 0x00cf92000000ffff
.pointer:
    dw $ - gdt32 - 1
    dd gdt32

gdt64:
    dq 0                                        ; zero entry
.code: equ $ - gdt64                            ; code segment
                                                ; executable | code | present | x86_64 code
    dq (1<<43) | (1<<44) | (1<<47) | (1<<53) 
.pointer:
    dw $ - gdt64 - 1
    dq gdt64                                    ; point to physical address of gdt