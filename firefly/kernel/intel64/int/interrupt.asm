bits 64

global interrupt_wrapper
global assign_cpu_exceptions

extern interrupt_handler
extern update

%macro popa64 0
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
%endmacro

%macro pusha64 0
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
%endmacro

%macro CPU_INTR 1
CPU_INTR%1:
    push 0
    push %1
    call interrupt_wrapper
%endmacro

%macro CPU_INTR_ERR 1
CPU_INTR_ERR%1:
    push %1
    call interrupt_wrapper
%endmacro

%macro register_handler 1
    lea rdi, [rel %1]
    mov rsi, 0x28
    mov rdx, 0x8E
    mov rcx, i
    call update
%endmacro

; - CPU Exceptions -
%assign i 0
%rep 8
    CPU_INTR i
%assign i i+1
%endrep

CPU_INTR_ERR 8
CPU_INTR_ERR 9
CPU_INTR_ERR 10
CPU_INTR_ERR 11
CPU_INTR_ERR 12
CPU_INTR_ERR 13
CPU_INTR_ERR 14

%assign i 15
%rep 17
    CPU_INTR i
%assign i i+1
%endrep
; - CPU Exceptions -

assign_cpu_exceptions:
    %assign i 0
    %rep 8
        register_handler CPU_INTR%+i
    %assign i i+1
    %endrep

    %rep 6
        register_handler CPU_INTR_ERR%+i
    %assign i i+1
    %endrep
    
    %assign i 15
    %rep 17
        register_handler CPU_INTR%+i
    %assign i i+1
    %endrep
    ret

; CPU exceptions
interrupt_wrapper:
    cld
    pusha64
    ; also save SSE state when we figure that out

    call interrupt_handler

    popa64
	add rsp, 16
    iretq
