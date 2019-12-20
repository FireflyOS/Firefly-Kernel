        ; keep file system from current fireflyos.bin
        ; and update only the bootloader code
        ; so we don't lose the files
[org 0]
        ; bootstrap code
    incbin "bootloader/build/mbr.bin", 0, 446
        ; keep partitions
    incbin "fireflyos.bin", 446, 66
        ; bootloader
    incbin "bootloader/bootloader.bin"
        ; zero out rest of hidden sectors
    times 1048576-($-$$) db 0
        ; jmp + OEM
    incbin "bootloader/build/vbr.bin", 0, 11
        ; keep FAT BPB
    incbin "fireflyos.bin",1048587, 51
        ; rest of boot sector
    incbin "bootloader/build/vbr.bin", 62
        ; rest of image
    incbin "fireflyos.bin", $
