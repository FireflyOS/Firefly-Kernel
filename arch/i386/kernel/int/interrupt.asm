; bits 32

; global interrupt_wrapper
; global exception_wrapper

; extern interrupt_handler
; extern exception_handler


; ; interrupt_wrapper:
; ;     pusha
; ;     ; also save SSE state when we figure that out
        
; ;     call interrupt_handler

; ;     popa
; ;     iret

; exception_wrapper:
;     pusha
;     ; also save SSE state when we figure that out

;     ; pass error code
;     push dword [esp + (13 * 8)]
;     call exception_handler

;     popa
;     iret

