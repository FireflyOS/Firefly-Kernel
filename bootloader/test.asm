	.file	"paging.cpp"
	.code16gcc
	.intel_syntax noprefix
	.text
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"binary size %d\r\n"
.LC1:
	.string	"fail"
	.text
	.globl	_Z11setup_pagesm
	.type	_Z11setup_pagesm, @function
_Z11setup_pagesm:
	push	ebp
	mov	ebp, esp
	push	ebx
	sub	esp, 12
	mov	ebx, DWORD PTR cluster_buffer
	push	DWORD PTR [ebp+8]
	push	OFFSET FLAT:.LC0
	call	_Z6printfPKcz
	add	esp, 12
	push	4096
	push	0
	push	ebx
	call	_Z6memsetPvci
	mov	BYTE PTR [ebx+4088], 3
	mov	BYTE PTR [ebx+4089], 16
	mov	BYTE PTR [ebx+4090], 2
	mov	BYTE PTR [ebx+4091], 0
	mov	BYTE PTR [ebx+4092], 0
	mov	BYTE PTR [ebx+4093], 0
	mov	BYTE PTR [ebx+4094], 0
	mov	BYTE PTR [ebx+4095], 0
	movq	xmm0, QWORD PTR [ebx+4088]
	movq	QWORD PTR [ebx], xmm0
	mov	DWORD PTR [esp], OFFSET FLAT:.LC1
	call	_Z3errPKc
	.size	_Z11setup_pagesm, .-_Z11setup_pagesm
	.ident	"GCC: (Ubuntu 7.4.0-1ubuntu1~18.04.1) 7.4.0"
	.section	.note.GNU-stack,"",@progbits
