extern _init_array_end
extern bmain

global start

section .start
bits 16
start:
    jmp bmain                       ; uh
                                    ; call global constructors
    lea bx, [rel _init_array_end-4]
    jmp .getaddr
.docall:
    call dx
.getaddr:
    mov dx, word [bx]
    sub bx, 4
    test dx, dx                     ; uh
    jnz .docall

    cld

    jmp bmain