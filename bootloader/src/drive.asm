[bits 16]
extern boot_vbr

global read
global write

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
    push bx
    push esi
    push word 0                     ; create disk address packet
    push word 0
    push word 0
    mov ax, word [bp+8]
    push ax
    push word [bp+4]
    push word [bp+6]
    push word [bp+10]
    push word 0x0010
    mov si, sp
    push ax                         ; keep # blocks to read for error checking
    mov ax, 0x4200                  ; extended read
    mov bx, [boot_vbr]              ; get boot drive
    mov dl, byte [bx+0x40]
    int 0x13                        ; read
    jc .err
    mov ax, word [si+8]             ; get block transferred
    pop dx
    cmp ax, dx                      ; is it what we expected?
    jne .err
    xor ax, ax
    jmp .done
.err:
    mov ax, 1
.done:
    add sp, 16
    pop esi
    pop bx
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
    push bx
    push esi
    push word 0                     ; create disk address packet
    push word 0
    push word 0
    mov ax, word [bp+8]
    push ax
    push word [bp+4]
    push word [bp+6]
    push word [bp+10]
    push word 0x0010
    mov si, sp
    push ax                         ; keep # blocks to read for error checking
    mov ax, 0x4300                  ; extended write
    mov bx, [boot_vbr]              ; get boot drive
    mov dl, byte [bx+0x40]    
    int 0x13                        ; write
    jc .err
    mov ax, word [si+8]             ; get blocks transferred
    pop dx
    cmp ax, dx                      ; is it what we expected?
    jne .err
    xor ax, ax
    jmp .done
.err:
    mov ax, 1
.done:
    add sp, 16
    pop esi
    pop bx
    pop bp
    ret
