bits 64

global load_gdt
load_gdt:
    lgdt [rdi]
    
    push 0x8
	lea rax, [rel .flush]
	push rax
    retfq
    .flush:
        mov ax, 0x10
        mov ds, ax
        mov es, ax
        mov ss, ax
        ; mov fs, ax
        ; mov gs, ax
        ret
