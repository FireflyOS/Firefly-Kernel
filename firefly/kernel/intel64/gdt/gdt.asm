bits 64

global load_gdt
load_gdt:
    lgdt [rdi]
    
    push 0x28
    push .flush
    retfq
    .flush:
        mov ax, 0x30
        mov ds, ax
        mov es, ax
        mov ss, ax
        mov fs, ax
        mov gs, ax
        ret
