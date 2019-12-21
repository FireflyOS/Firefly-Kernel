bits 16
extern boot_vbr

global read
global write

section .text
;;
; @fn                       unsigned short read(void *dest, unsigned int segment, unsigned int sector, unsigned int amt)
;                           Reads an amount of sectors from the drive into dest.
; @param dest               Offset to read the data into.
; @param segment            Segment to read the data into.
; @param sector             LBA to begin reading from.
; @param amt                The number of sectors to read.
; @return                   result of attempt to read from the drive
;         0                 success
;         1                 failure
read:
    push bp
    mov bp, sp
    push esi
    mov ax, word [bp+6]
    mov word [address_packet+4], ax
    mov ax, word [bp+10]
    mov word [address_packet+6], ax
    mov ax, word [bp+14]
    mov word [address_packet+8], ax
    mov ax, word [bp+18]
    mov word [address_packet+2], ax
    mov si, address_packet
    push ax                         ; keep # blocks to read for error checking
    mov ax, 0x4200                  ; extended read
    mov dl, byte [0xfe40]            ; get boot drive
    int 0x13                        ; read
    jc .err
    mov ax, word [address_packet+2] ; get block transferred
    pop dx
    cmp ax, dx                      ; is it what we expected?
    jne .err
    mov ax, 1
    jmp .done
.err:
    xor ax, ax
.done:
    pop esi
    pop bp
    ret

;;
; @fn                       unsigned short write(void *src, unsigned int segment, unsigned int sector, unsigned int amt)
;                           Writes an amount of sectors from src into the drive.
; @param dest               Offset of data to write
; @param segment            Segment of data to write.
; @param sector             LBA to write to.
; @param amt                The number of sectors to write.
; @return                   result of attempt to write to the drive
;         0                 success
;         1                 failure
write:
    push bp
    mov bp, sp
    push esi
    mov ax, word [bp+6]
    mov word [address_packet+4], ax
    mov ax, word [bp+10]
    mov word [address_packet+6], ax
    mov ax, word [bp+14]
    mov word [address_packet+8], ax
    mov ax, word [bp+18]
    mov word [address_packet+2], ax
    mov si, address_packet
    push ax                         ; keep # blocks to read for error checking
    mov ax, 0x4300                  ; extended write
    mov dl, byte [0xfe40]            ; get boot drive    
    int 0x13                        ; write
    jc .err
    mov ax, word [address_packet+2] ; get blocks transferred
    pop dx
    cmp ax, dx                      ; is it what we expected?
    jne .err
    mov ax, 1
    jmp .done
.err:
    xor ax, ax
.done:
    pop esi
    pop bp
    ret

address_packet:
    db 0x10                         ; packet size
    db 0                            ; reserved
    dw 0                            ; # blocks to transfer
    dd 0                            ; destination buffer
    dq 0                            ; starting LBA