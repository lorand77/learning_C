	.file	"sum-stack.c"
	.text
	.p2align 4
	.globl	sum
	.type	sum, @function
sum:
.LFB0:
	.cfi_startproc
	endbr64
	movl	16(%rsp), %eax
	addl	8(%rsp), %eax
	ret
	.cfi_endproc
.LFE0:
	.size	sum, .-sum
	.section	.text.startup,"ax",@progbits
	.p2align 4
	.globl	main
	.type	main, @function
main:
.LFB1:
	.cfi_startproc
	endbr64
	xorl	%eax, %eax
	ret
	.cfi_endproc
.LFE1:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 15.2.0-16ubuntu1) 15.2.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
