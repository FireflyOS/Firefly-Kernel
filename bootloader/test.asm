	.file	"bmain.cpp"
	.code16gcc
	.text
	.globl	bmain
	.type	bmain, @function
bmain:
.LFB0:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$8, %esp
	call	read_mmap
	pushl	$1
	pushl	$0
	pushl	$0
	pushl	buffer
	call	read
	addl	$16, %esp
	testw	%ax, %ax
	je	.L2
	call	_Z3errv
.L2:
	movl	buffer, %eax
	cmpw	$-21931, 510(%eax)
	jne	.L3
	movl	$3667, %eax
#APP
# 21 "src/bmain.cpp" 1
	int $0x10

# 0 "" 2
#NO_APP
.L3:
.L4:
	jmp	.L4
	.cfi_endproc
.LFE0:
	.size	bmain, .-bmain
	.globl	buffer
	.data
	.align 4
	.type	buffer, @object
	.size	buffer, 4
buffer:
	.long	64512
	.ident	"GCC: (Ubuntu 7.4.0-1ubuntu1~18.04.1) 7.4.0"
	.section	.note.GNU-stack,"",@progbits
