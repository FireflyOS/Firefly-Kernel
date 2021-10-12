bits 64

global ide_read_buffer0
ide_read_buffer0:
    push es
    mov ax, ds
    mov es, ax
    ret

global ide_read_buffer1  
ide_read_buffer1:
    pop es
    ret