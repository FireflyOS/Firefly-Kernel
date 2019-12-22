bits 16

global copy_unreal

section .text
;;
; @fn                       void copy_unreal(unsigned long src, unsigned long dest, unsigned long amt);
;                           Copies data to any address above 64K.
; @param src                Offset of src address
; @param dest               destination address
; @param amt                Number of bytes to copy

copy_unreal:
    push bp
    mov bp, sp
    push ebx
    mov ecx, dword [bp+14]
.next:
    sub ecx, 4
    mov ebx, dword [bp+6]
    mov eax, dword [ebx + ecx]
    mov ebx, dword [bp+10]
    mov dword [ebx + ecx], eax
    cmp ecx, 0
    jnz .next
    pop ebx
    pop bp
    ret