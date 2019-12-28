extern _init_array_end
extern kmain
global start

bits 64

section .start
start:
    mov rsp, stack_top
    ;cli
    push rdi
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
;    cmp r12, -1                                ; Should we be testing for 0 or -1?
    jnz .docall

    cld

    pop rdi
    jmp kmain

section .bss
    resb 65536
stack_top: