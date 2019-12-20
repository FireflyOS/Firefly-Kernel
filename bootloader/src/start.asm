extern _init_array_end
extern bmain

global start

section .start
bits 16
start:
                                    ; call global constructors
    lea bx, [rel _init_array_end-2]
    jmp .getaddr
.docall:
    call dx
.getaddr:
    mov dx, word [bx]
    sub bx, 2
    test dx, dx
    jnz .docall

    cld

    jmp bmain