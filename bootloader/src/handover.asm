;VIRT_ADDR equ 0xFFFFFFFF80000000

bits 16

global handover

section .text

handover:
    lgdt [gdt32.pointer]
    mov eax, cr0                    ; jump to protected
    or al, 1
    mov cr0, eax
    jmp gdt32.code:.stub32

bits 32
.stub32

    mov esp, stack_top
    call check_cpuid
    call check_long_mode
                                                ; enable long mode and paging
    mov eax, cr4
    or eax, 0b110000                            ; set PAE and PSE
    mov cr4, eax

    mov eax, 0x00020000                         ; point cr3 to pml4
    mov cr3, eax

    mov ecx, 0xC0000080                         ; get EFER MSR
    rdmsr
    or eax, 1 << 8                              ; set LME
    wrmsr

    mov eax, cr0
    or eax, 1 << 31                             ; set PG
    mov cr0, eax

    lgdt [gdt64.pointer]
    jmp gdt64.code:.stub64

bits 64
.stub64:                                        ; trampoline to higher half
    mov rsp, stack_top
    mov rax, qword long_mode_start
    jmp rax

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

error:                                          ; Prints `ERR: ` and the given error code to screen and hangs.
                                                ; parameter: error code (in ascii) in al
    mov dword [0xb8000], 0x4f524f45
    mov dword [0xb8004], 0x4f3a4f52
    mov dword [0xb8008], 0x4f204f20
    mov byte  [0xb800a], al
    hlt

bits 64
long_mode_start:
    mov ax, 0
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    jmp 0x00100000

section .bss
    resb 4096
stack_top:

section .rodata
gdt32:
    dq 0
.code: equ $ - gdt32
    dq 0xffff00009acf00
.data: equ $ - gdt32
    dq 0xffff000092cf00
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