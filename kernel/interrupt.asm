bits 64

global init_idt
global interrupt_wrapper
global exception_wrapper

;extern idt
;extern _GLOBAL__sub_I_idt.cpp
extern interrupt_handler
extern exception_handler

;init_idt:
 ;   call _GLOBAL__sub_I_idt.cpp
  ;  mov rax, idt
   ; lidt [rax]
    ;ret

interrupt_wrapper:
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    ; should probably also save SSE state

    call interrupt_handler

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    iret

exception_wrapper:
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    ; should probably also save SSE state

    ; pass error code
    push qword [rsp + (13 * 8)]
    call exception_handler

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    iret