extern _init_array_end
extern kmain
global start

bits 64

section .start
start:
                                                ; call global constructors
    lea rbx, [rel _init_array_end-8]
                                                ; Since all of our code and data is within +/-2GiB
                                                ; we can use a RIP relative instruction with disp32
    jmp .getaddr
.docall:
    call r12
.getaddr:
    mov r12, [rbx]
    sub rbx, 8
    test r12, r12
    jnz .docall

    cld

    jmp kmain