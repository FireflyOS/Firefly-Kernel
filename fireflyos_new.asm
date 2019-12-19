                            ; creates a brand new image
                            ; destroys existing files in current fireflyos.bin
[org 0]
    incbin "bootloader/build/mbr.bin"
    incbin "bootloader/bootloader.bin"

                            ; 1048576 0's
    times 131072-($-$$) dq 0

    incbin "bootloader/build/vbr.bin"
                            ; 1536 0's
    times 192 dq 0
    incbin "bootloader/build/vbr.bin"

                            ; 2128896 0's
    times 266112-($-$$) dq 0

    dd 0x0fff_fff8          ; partitioned disk FAT ID
    dd 0xffff_ffff          ; end of chain marker
    times 4 dd 0x0fff_ffff
    
                            ; 135265792 0's
    times 16908224-($-$$) dq 0


; broken, don't use