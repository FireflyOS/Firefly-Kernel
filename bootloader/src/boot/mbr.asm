[bits 16]

[org 0x1500]                            ; mbr will be moved here,
                                        ; and we don't need the current address until we jump to it

    cli                                 ; no interrupts while setting up
startup:
    mov bp, 0x1400                      ; set up initial values
    mov sp, bp
    xor ax, ax
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    sti

    mov si, 0x7c00                      ; move this sector out of the way
    mov di, 0x1500
    mov cx, 512
    rep movsb
    jmp 0000:read_parts                 ; ensure 0000 is in cs and jump to continue code

read_parts:
    mov bx, 0x7dbe                      ; address of status of first partition

    mov al, byte [bx]                   ; get status
    cmp al, 0x80                        ; active?
    je chain                            ; yes, chain load it

    add bx, 16                          ; next partition
    mov al, byte [bx]                   ; get status
    cmp al, 0x80                        ; active?
    je chain                            ; yes, chain load it

    add bx, 16                          ; next partition
    mov al, byte [bx]                   ; get status
    cmp al, 0x80                        ; active?
    je chain                            ; yes, chain load it

    add bx, 16                          ; next partition
    mov al, byte [bx]                   ; get status
    cmp al, 0x80                        ; active?
    jne no_active                       ; no, err

chain:                                  ; load vbr of active partition
    mov ah, 0x41                        ; first check if int 13h extensions are supported
    push bx                             ; save partition entry address
    mov bx, 0x55aa
    int 0x13
    jc no_ext
    test cx, 1                          ; extended read functions?
    jz no_ext                           ; err if not installed
    pop bx                              ; restore entry address

    mov eax, dword [bx+8]               ; set LBA of active partition
    mov dword [address_packet+8], eax
    mov si, address_packet
    mov ah, 0x42                        ; read LBA (dl still has drive number)
    int 0x13
    jc read_error                       ; error reading
    mov ax, word [address_packet+2]     ; get block count
    cmp ax, 1                           ; is it 1?
    jne read_error                      ; no, error reading

    jmp 0x7c00                          ; jump to loaded VBR

no_active:
    mov si, no_active_msg
    jmp print

no_ext:
    mov si, no_ext_msg
    jmp print

read_error:
    mov si, read_error_msg

print:                                  ; print null-terminated message
    mov ah, 0x0e                        ; pointed to by si
.next:
    lodsb
    test al, al
    jz .done
    int 0x10
    jmp .next
.done:
    hlt
    jmp .done


no_active_msg:  db 'No active partition was found.', 0x0d, 0x0a, 0
no_ext_msg:     db 'int 13h extensions are not supported.', 0x0d, 0x0a, 0
read_error_msg: db 'An error occurred while reading the disk.', 0x0d, 0x0a, 0

address_packet:
    db 0x10                             ; packet size
    db 0                                ; reserved
    dw 1                                ; # blocks to transfer
    dd 0x7c00                           ; destination buffer
    dq 0                                ; starting LBA

    times 0x1be-($-$$) db 0

    db 0x80
    db 8, 9, 1
    db 0x0c
    db 0x19, 0x20, 0x99
    dd 32768
    dd 262144

    times 510-($-$$) db 0
    dw 0xaa55
