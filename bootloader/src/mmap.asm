bits 16
global read_mmap
global count_free

section .text

;; @fn                      void read_mmap()
;                           Uses int 15h, ax=e820 to construct a list of memory ranges.
;                           Only free ranges will be kept and the list is terminated
;                           by a range whose length is 0.
read_mmap:
    push bx
    push di
    push es
    mov ax, 0x1000
    mov es, ax                  ; read into segment 1
    xor ax, ax
    mov di, ax                  ; offset 0
    xor ebx, ebx
    mov eax, 0xe820
    mov ecx, 20
    mov edx, 0x534d4150
.next:
    int 0x15
    jc .err
    push eax
    mov eax, dword [es:di + 16]
    cmp eax, 1                  ; skip non-free ranges
    jne .skip
    add di, 20
.skip:
    pop eax
    test ebx, ebx
    jz .done
    mov eax, 0xe820
    mov ecx, 20
    jmp .next
.err:
    mov ebx, 0xb8000
    mov byte [ebx], 'E'
.loop:
    hlt
    jmp .loop
.done:
    xor eax, eax
    mov dword [es:di + 8], eax
    mov dword [es:di + 12], eax
    pop es
    pop di
    pop bx
    ret

;; temporary function to print the number of free ranges
count_free:
    push bx
    mov dl, '0'
    mov ebx, 0x10008
.next:
    mov eax, dword [ebx]
    test eax, eax
    jz .done
    add ebx, 20
    inc dl
    jmp .next
.done:
    mov ebx, 0xb8000            ; test unreal
    mov byte [ebx], dl
    pop bx
    ret