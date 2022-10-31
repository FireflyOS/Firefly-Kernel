bits 64
section .text

global enterUserspace
enterUserspace:
	swapgs ; swap kernel GS

    ; setup user stack
    add rsi, 8192
    mov rbp, rsi
    mov rsp, rbp

    ; NOTE: SS & CS are set by sysret
    ; Set user segment selectors
    mov ax, 0x18
    mov ds, ax
    mov fs, ax
    mov gs, ax
    mov es, ax

    ; Enter ring3
    mov rcx, rdi   ; RIP
	mov r11, 0x202 ; RFLAGS
    o64 sysret
