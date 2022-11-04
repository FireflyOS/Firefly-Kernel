bits 64

%include "../firefly/kernel/intel64/defs.mac"

global assign_cpu_exceptions
global assign_irq_handlers

extern interrupt_handler
extern irq_handler
extern ipi_handler
extern update

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
    mov rsi, 0x8
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

; IRQs

%macro CPU_IRQ 1
CPU_IRQ%1:
    cld
    push 0
    pusha64
    mov rdi, %1
    mov rsi, rsp
    xor rdx, rdx
    xor rbp, rbp
    call irq_handler
    popa64
        add rsp, 8
    iretq
%endmacro

%macro CPU_IPI 1
CPU_IPI%1:
    cld
    push 0
    pusha64
    mov rdi, %1
    mov rsi, rsp
    xor rdx, rdx
    xor rbp, rbp
    call ipi_handler
    popa64
    	add rsp, 8
    iretq
%endmacro

%assign i 32
%rep 16
CPU_IRQ i
%assign i i+1
%endrep

%assign i 48
%rep 256-48
CPU_IPI i
%assign i i+1
%endrep

assign_irq_handlers:
    %assign i 32
    %rep 16
    	register_handler CPU_IRQ%+i
    %assign i i+1
    %endrep
    %assign i 48
    %rep 256-48
    	register_handler CPU_IPI%+i
    %assign i i+1
    %endrep
    ret

