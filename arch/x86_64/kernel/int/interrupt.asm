; bits 64

; global interrupt_wrapper
; global exception_wrapper

; extern interrupt_handler
; extern exception_handler

; %macro popa64 0
;     pop r15
;     pop r14
;     pop r13
;     pop r12
;     pop r11
;     pop r10
;     pop r9
;     pop r8
;     pop rdi
;     pop rsi
;     pop rdx
;     pop rcx
;     pop rbx
;     pop rax
; %endmacro

; %macro pusha64 0
;     push rax
;     push rbx
;     push rcx
;     push rdx
;     push rsi
;     push rdi
;     push r8
;     push r9
;     push r10
;     push r11
;     push r12
;     push r13
;     push r14
;     push r15
; %endmacro

; interrupt_wrapper:
;     cld
;     pusha64
;     ; also save SSE state when we figure that out
        
;     call interrupt_handler

;     popa64
;     iretq

; exception_wrapper:
;     cld
;     pusha64
;     ; also save SSE state when we figure that out

;     .loop:
;         hlt
;         jmp .loop
;     ; pass error code
;     push qword [rsp + (13 * 8)]
;     call exception_handler

;     popa64
;     iretq