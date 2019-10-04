bits 64

global _fini
global _init

section .init_start

_init:
    push rbp
    mov rbp, rsp

section .fini_start
_fini:
    push rbp
    mov rbp, rsp