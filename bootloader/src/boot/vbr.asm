[bits 16]
[org 0xfe00]
BLOADER_OFFSET equ 0x1400

    jmp short startup               ; make sure we stay in the same sector
    nop                             ; jmp short is only 2 bytes

    db 'LIME    '                   ; OEM
    dw 512                          ; bytes per sector
    db 8                            ; sectors per cluster
    dw 2110                         ; # reserved sectors
    db 2                            ; # FATs
    dw 0                            ; max # root dir entries
    dw 0                            ; total # sectors
    db 0xf8                         ; media descriptor (0xf8 = hard disk partition)
    dw 0                            ; sectors per FAT
                                    ; dos 3.31 bpb
    dw 63                           ; sectors per track
    dw 256                          ; # heads
    dd 2048                         ; # hidden sectors (before this partition)
    dd 15728640                     ; # sectors for FAT32
                                    ; fat32 ebpb
    dd 15329                        ; sectors per fat
    dw 0                            ; drive description / mirroring flags
    dw 0                            ; version
    dd 2                            ; cluster number of root dir start
    dw 1                            ; logical sector of fs info sector
    dw 6                            ; logical sector of copy of FAT boot sectors
    times 12 db 0                   ; reserved
bootdr:
    db 0x80                         ; boot drive number
    db 0                            ; reserved
    db 0x29                         ; extended boot signature
    dd 0x3a97a48e                   ; volume ID
    db 'FIREFLYOS  '                ; volume label
    db 'FAT32   '                   ; file system type

startup:
    mov bp, 0x1400                  ; set up initial values
    mov sp, bp
    xor ax, ax
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov si, 0x7c00                  ; move this sector out of the way
    mov di, 0xfe00
    mov cx, 512
    rep movsb
    jmp 0000:read_kernel            ; jump to new location

read_kernel:
                                    ; read bootloader into BLOADER_OFFSET
    mov byte [bootdr], dl           ; keep boot drive num

    mov ah, 0x42
    mov si, address_packet
    int 0x13
    jc error                        ; error reading
    mov ax, word [address_packet+2] ; get block count
    cmp ax, 116                     ; is it 116?
    jne error                       ; no, error reading

    cli
    call enable_a20
    call enable_unreal
    sti

    jmp BLOADER_OFFSET              ; go to entry point

enable_a20:                         ; enable the A20 line
    mov ax, 0xffff                  ; first test if BIOS already enabled it
    mov ds, ax
    mov di, 0x7dfe
    mov si, 0x7e0e
    mov al, byte [es:di]
    cmp al, byte [ds:si]
    jne .done                       ; already enabled if not equal
                                    ; otherwise it may be by chance
    add byte [es:di], 1             ; so change value in lower memory and check again
    mov al, byte [es:di]
    cmp al, byte [ds:si]
    jne .done                       ; if different now then for sure enabled

    call a20wait                    ; otherwise enable it
    mov al, 0xad
    out 0x64, al

    call a20wait
    mov al, 0xd0
    out 0x64, al

    call a20wait2
    in al, 0x60
    push ax

    call a20wait
    mov al, 0xd1
    out 0x64, al

    call a20wait
    pop ax
    or al, 2
    out 0x60, al

    call a20wait
    mov al, 0xae
    out 0x64, al

    call a20wait

    mov cx, 100                     ; make sure it's enabled, loop 100 times cause keyboard method may be slow
    mov ax, 0xffff
    mov ds, ax
    mov di, 0x7dfe
    mov si, 0x7e0e

.ensure:
    mov al, byte [es:di]
    cmp al, byte [ds:si]
    jne .done                       ; already enabled if not equal
                                    ; otherwise it may be by chance
    add byte [es:di], 1             ; so change value in lower memory and check again
    mov al, byte [es:di]
    cmp al, byte [ds:si]
    jne .done                       ; if different now then for sure enabled

    loop .ensure

    jmp error                       ; still not enabled, give up

.done:
    xor ax, ax
    mov ds, ax
    ret

a20wait:
    in al, 0x64                     
    test al, 2
    jnz a20wait
    ret

a20wait2:
    in al, 0x64                     
    test al, 1
    jnz a20wait
    ret

enable_unreal:                      ; switch to unreal mode
    push ds
    lgdt [gdt32info]
    mov eax, cr0
    or al, 1
    mov cr0, eax
    jmp $+2
    mov bx, 0x08
    mov ds, bx
    mov eax, cr0                    ; also enable SSE
    and ax, 0xFFFB                  ; clear coprocessor emulation CR0.EM
    or ax, 0x2                      ; set coprocessor monitoring  CR0.MP
    mov cr0, eax
    mov eax, cr4
    or ax, 3 << 9                   ; set CR4.OSFXSR and CR4.OSXMMEXCPT at the same time
    mov cr4, eax
    mov eax, cr0
    and eax, 0xfffffffe
    mov cr0, eax
    pop ds
    ret

error:
    mov ah,0x0e
    mov al, 'E'
    int 0x10
    mov al, 'R'
    int 0x10
    mov al, 'R'
    int 0x10
.loop:
    hlt
    jmp .loop

address_packet:
    db 0x10                         ; packet size
    db 0                            ; reserved
    dw 116                          ; # blocks to transfer
    dd BLOADER_OFFSET               ; destination buffer
    dq 1                            ; starting LBA

gdt32info:
   dw gdt32_end - gdt32 - 1
   dd gdt32
 
gdt32:
   dq 0                             ; null entry

   db 0xff                          ; data segment
   db 0xff
   db 0
   db 0
   db 0
   db 10010010b
   db 11001111b
   db 0
gdt32_end:

    times 510-($-$$) db 0
    dw 0xaa55

                                    ; fs info sector
    db 0x52, 0x52, 0x61, 0x41       ; signature
    times 480 db 0                  ; reserved
    db 0x72, 0x72, 0x41, 0x61       ; signature
    dd 0x001deffc                   ; # free clusters
    dd 6                            ; idx of most recently allocated cluster
    times 14 db 0                   ; reserved
    dw 0xaa55

    times 510 db 0                  ; third fat32 boot sector
    dw 0xaa55